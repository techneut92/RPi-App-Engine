class RpaeApp{
    // DEFINES (sort of)
    #DEFAULT_TARGET = 'serverApp';
    #APPTYPE = 'clientApp';

    // Attributes
    #ws;
    #appId;
    #host;
    #peers = {};
    #queue = [];
    #appTypes = ['all', 'clientApp', 'serverApp', 'unknownType', 'uid'];

    constructor(appId) {
        this.#appId = appId;
    }

    #connect(host='ws://localhost:9738'){
        this.#ws = new WebSocket(host);
        this.#ws.onopen = this.#onOpen;
        this.#ws.onmessage = this.#handshake;
        this.#ws.onclose = this.#onDisconnect;
        this.#ws.onerror = this.#onError;
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
            this.#handleServerMessages(JSON.parse(msg)[1]['msgData']);
            this.#handleServerMessages(JSON.parse(msg)[2]['msgData']);
            this.#ws.onmessage = this.#onMessage;
        }
    }

    #handleServerMessages(msg){

    }

    #onMessage(message){
        console.log(message);
        let msg = JSON.parse(message.data);
        this.onMessage(msg);
    }

    #onOpen(data){
        console.log(data);
        this.onOpen();
    }

    #onDisconnect(data){
        console.log(data);
        this.onDisconnect(data);
    }

    #onNewPeer(peer){
        this.onNewPeer(peer);
    }

    #onPeerDisconnect(peer){
        this.onPeerDisconnect(peer);
    }

    #onError(err){
        this.onError(err);
    }

    onOpen(){}
    onMessage(message){}
    onDisconnect(info){}
    onError(error){}
    onNewPeer(peer){}
    onPeerDisconnect(peer){}

    #genPackage(message, target, uid=null){
        if (target == null) target = this.#DEFAULT_TARGET;
        else if (target === 'uid' && uid != null){

        }
    }

    sendMessage(message, target=null, uidTarget=null){
        if (this.isReady() === 'OPEN')
            this.#ws.send(this.#genPackage(message, target, uidTarget));
        else
            let a = null // TODO REPLACE with adding to queue
    }

    get isReady(){ return this.#ws.readyState === 'OPEN'; }

    get WS_Status() { return this.#ws.readyState; }
    get appTypes() { return this.#appTypes; }
    get host(){ return this.#host }

    set host(value){
        this.#host = value;
        if (this.isReady)
            this.#ws.close();
        this.#connect(value);
    }
}