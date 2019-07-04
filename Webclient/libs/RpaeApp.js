class RpaeApp{
    // DEFINES (sort of)
    #DEFAULT_TARGET = 'serverApp';
    #APPTYPE = 'clientApp';

    // Attributes
    #ws;
    #appId;
    #host;
    #peers = {};
    #recQueue = [];
    #sendQueue = [];
    #appTypes = ['all', 'clientApp', 'serverApp', 'unknownType', 'uid'];
    #connected = false;
    #isReady = false;
    #onOpenData;

    constructor(appId) {
        this.#appId = appId;
    }

    #connect(host='ws://localhost:9738'){
        this.#ws = new WebSocket(host);
        this.#ws.onopen = this.#onOpen;
        this.#ws.onmessage = this.#handshake;
        this.#ws.onclose = this.#onDisconnect;
        this.#ws.onerror = this.#onError;
        this.#ws.connect();
    }

    #handshake(message){
        console.log(message);
        if (message.data === 'HANDSHAKE'){
            console.log('init server handshake...');
            let msg = {
                'id': this.#appId,
                'appType': this.#APPTYPE
            };
            this.#ws.send(JSON.stringify(msg));
        }
        else if (message.data.startsWith('HANDSHAKE_SUCCES')){
            let msg = message.data.split(' ');
            this.#handleServerMessages(JSON.parse(msg[1])['msgData']);
            this.#handleServerMessages(JSON.parse(msg[2])['msgData']);
            this.#ws.onmessage = this.#onMessage;
            this.onOpen(this.#onOpenData);
            while(this.#recQueue.length > 0)
                this.#onMessage(this.#recQueue.pop());
            this.#isReady = true;
            console.log('Handshake successful!');
        }
        else if (message.data.startsWith("HANDSHAKE_FAILURE")){
            this.#onError(message.data);
        }
        else {
            this.#recQueue.push(message);
            if (this.#isReady)
                while(this.#recQueue.length > 0)
                    this.#onMessage(this.#recQueue.pop());
        }
    }

    #handleServerMessages(msg){
        msg = JSON.parse(msg);
        switch (msg['action']){
            case 'newClient':
                this.#onNewPeer(msg['client']);
                break;
            case 'initSelf':
                break;
            case 'getClients':
                break;
            case 'clientDisconnected':
                this.#onPeerDisconnect(msg['clientUid']);
                break;
        }
    }

    #onMessage(message){
        console.log(message);
        let msg = JSON.parse(message.data);
        this.onMessage(msg);
    }

    #onOpen(data){
        console.log(data);
        this.#onOpenData = data;
        this.#connected = true;
    }

    #onDisconnect(data){
        console.log(data);
        this.onClose(data);
    }

    #onNewPeer(peer){
        this.onNewPeer(peer);
    }

    #onPeerDisconnect(peerUid){
        this.onPeerDisconnect(peerUid);
    }

    #onError(err){
        this.onError(err);
    }

    onOpen(){}
    onMessage(message){}
    onClose(info){}
    onError(error){}
    onNewPeer(peer){}
    onPeerDisconnect(peer){}

    #genPackage(message, target=#DEFAULT_TARGET, uid=null){
        let pck = {
            'serverTarget': target,
            'msgData': message
        };
        if (target !== #DEFAULT_TARGET){
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

    sendMessage(message, target=null, uidTarget=null){
        if (this.#isReady && this.#sendQueue.length > 0){
            let msgdata = this.#sendQueue.pop();
            this.#ws.send(msgdata[0], msgdata[1], msgdata[2]);
            this.sendMessage(message, target, uidTarget);
        }
        else if (this.#isReady)
            this.#ws.send(this.#genPackage(message, target, uidTarget));
        else
            this.#sendQueue.push([message, target, uidTarget]);
    }

    get WS_Status() { return this.#ws.readyState; }
    get appTypes() { return this.#appTypes; }
    get host(){ return this.#host }

    set host(value){
        this.#host = value;
        if (this.#isReady)
            this.#ws.close();
        this.#connect(value);
    }
}