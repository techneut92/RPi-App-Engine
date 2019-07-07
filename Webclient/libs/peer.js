class Peer{
    __appID = null;
    __location = null;
    __uid = null;
    __appType = null;
    __peerAddress = null;
    __peerName = null;
    __peerOrigin = null;
    __updateSelfDone = false;
    
    constructor(appID){
        this.__appID = appID;
    }

    __updateSelf(data){
        if (!this.__updateSelfDone) {
            if (data['appId'] !== undefined)
                this.__appID = data['appId'];
            if (data['location'] !== undefined)
                this.__location = data['location'];
            this.__appType = data['appType'];
            this.__uid = data['uid'];
            this.__peerAddress = data['peerAddress'];
            this.__peerOrigin = data['peerOrigin'];
            this.__peerName = data['peerName'];
            this.__location = '';
            this.__updateSelfDone = true;
        }
    }

    get appID() { return this.__appID;}
    get location() { return this.__location;}
    get uid() { return this.__uid;}
    get appType() { return this.__appType;}
    get peerAddress() { return this.__peerAddress;}
    get peerName() { return this.__peerName;}
    get peerOrigin() { return this.__peerOrigin;}
    //get updateSelfDone (){ return this.__updateSelfDone; }
}