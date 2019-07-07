class RpaeApp extends Peer{
    __DEFAULT_TARGET = 'serverApp';
    __APPTYPE = 'clientApp';

    __websocket = null;
    __host = null;
    __peers = {};
    connected = false;
    __isReady = false;
    __recQueue = [];
    __sendQueue = [];
    __onOpenData = null;
    __appTypes = ['all', 'clientApp', 'serverApp', 'unknownType', 'uid'];

    constructor(appID){
        super(appID);
    }

    __connectSocket(host="ws://localhost:9738") {
        let ws = new WebSocket(host);
        ws.onopen = (event) => this.__onOpen(event);
        ws.onmessage = (message) => this.__handshake(message);
        ws.onclose = (event) => this.__onClose(event);
        ws.onerror = (event) => this.__onError(event);
        return ws;
    }

    __onOpen(data){
        this.__connected = true;
        this.__onOpenData = data;
    }
    
    __onClose(data){
        // TODO from data check why it disconnected and attempt reconnects if necessary.
        this.__peers = {};
        this.__uid = null;
        this.__peerAddress = null;
        this.__peerName = null;
        this.__peerOrigin = null;
        this.__updateSelfDone = false;
        this.__connected = false;
        this.__isReady = false;
        this.onClose(data);
    }
    
    __onError(err){
        // TODO ATTEMPT RECONNECT AFTER x SECONDS
        this.onError(err);
    }

    __onMessage(message){
        try {
            let msg = JSON.parse(message.data);
            if (msg['originName'] !== undefined && msg['originName'] === 'server')
                this.__handleServerMessages(msg['msgData']);
            else
                this.onMessage(msg['msgData'], this.__peers[msg['origin']['uid']])
        }
        catch (e){
            console.log('error parsing message in ' + this.__appID + ' uid: ' + this.uid);
            console.log(e);
        }

    }

    __onNewPeer(peer){
        let p = new Peer();
        p.__updateSelf(peer);
        this.__peers[p.uid] = p;
        this.onNewPeer(p);
    }

    __onPeerDisconnect(peerUid) {
        let p = this.__peers[peerUid];
        delete this.__peers[peerUid];
        this.onPeerDisconnect(p);
    }

    __handleServerMessages(msg){
        msg = JSON.parse(msg);
        switch (msg['action']){
            case 'newClient':
                this.__onNewPeer(msg['client']);
                break;
            case 'initSelf':
                this.__updateSelf(msg['self']);
                break;
            case 'getClients':
                for (let i = 0; i < msg['clients'].length; i++) {
                    this.__peers[msg['clients'][i]['uid']] = new Peer();
                    this.__peers[msg['clients'][i]['uid']].__updateSelf(msg['clients'][i]);
                }
                break;
            case 'clientDisconnected':
                this.__onPeerDisconnect(msg['clientUid']);
                break;
        }
    }

    __handshake(message){
        if (message.data === 'HANDSHAKE'){
            let msg = {
                'id': this.__appID,
                'appType': this.__APPTYPE
            };
            this.__websocket.send(JSON.stringify(msg));
        }
        else if (message.data.startsWith('HANDSHAKE_SUCCES')){
            let msg = message.data.split(' ');
            this.__handleServerMessages(JSON.parse(msg[1])['msgData']);
            this.__handleServerMessages(JSON.parse(msg[2])['msgData']);
            this.__websocket.onmessage = (message) => this.__onMessage(message);
            while(this.__recQueue.length > 0)
                this.__onMessage(this.__recQueue.pop());
            this.__isReady = true;
            this.onOpen(this.__onOpenData);
            if (this.__sendQueue.length > 0) {
                let msgdata = this.__sendQueue.pop();
                this.__websocket.send(this.__genPackage(msgdata[0],
                    msgdata[1], msgdata[2]));
            }
        }
        else if (message.data.startsWith("HANDSHAKE_FAILURE")){
            this.__onError(message.data);
        }
        else {
            this.__recQueue.push(message);
            if (this.__isReady)
                while(this.__recQueue.length > 0)
                    this.__onMessage(this.__recQueue.pop());
        }
    }

    __genPackage(message, target = this.__DEFAULT_TARGET, uid = null) {
        let pck = {
            'serverTarget': target,
            'msgData': message
        };
        if (target !== this.__DEFAULT_TARGET) {
            pck.serverTarget = target;
            if (uid != null && target === 'uid')
                pck['uid'] = uid;
            else if (uid == null && target === 'uid') {
                console.log('ERROR: trying to send message to specific uid but no uid is given');
                return null;
            }
        }
        return JSON.stringify(pck);
    }

    sendMessage(message, target=this.__DEFAULT_TARGET, uidTarget=null){
        if (this.isReady && this.__sendQueue.length > 0) {
            let msgdata = this.__sendQueue.pop();
            this.sendMessage(this.__genPackage(msgdata[0],
                msgdata[1], msgdata[2]));
            this.sendMessage(message, target, uidTarget);
        } else if (this.__isReady) {
            this.__websocket.send(this.__genPackage(message, target, uidTarget));
        } else
            this.__sendQueue.push([message, target, uidTarget]);
    }

    openSocket(){ this.host = this.__host; }
    closeSocket(){ this.__websocket.close(); }

    // functions to be overridden by the new class
    onOpen(data) {}
    onClose(data) {}
    onError(err) {}
    onNewPeer(peer) {}
    onPeerDisconnect() {}

    get appTypes() { return this.__appTypes; }
    get peers() { return this.__peers; }
    get isReady() {return this.__isReady; }
    get isConnected() { return this.__connected; }

    get host() {return this.__host;}
    set host(value){
        this.__host = value;
        if (this.__connected) {
            this.__isReady = false;
            this.__connected = false;
            this.__websocket.close();
        }
        this.__websocket = this.__connectSocket(value);
    }
}