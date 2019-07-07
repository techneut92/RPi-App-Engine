class ExampleApp extends RpaeApp{
    constructor(){
        super('ExampleApp');                    // Set your AppID here
        this.host = 'ws://192.168.2.8:9738';    // Setting host will immediately start up the connection.
    }

    onOpen(data){
        console.log('Available peers: ', this.peers);
        console.log('My app ID:', this.appID);
        console.log('My unique ID:', this.uid); // uid is and unique id given by the server.
        console.log('Number of connected peers:', Object.keys(this.peers).length);
        console.log('Connected with:', this.host);
        console.log('My app type:', this.appType);
        console.log('Peer address:', this.peerAddress);
        console.log('Peer name', this.peerName);
        console.log('Connected as:', this.peerOrigin);
        console.log('isReady will be true once the connection is established AND had an successful handshake', this.isReady);
        console.log('The apptypes usuable for sendMessage:', this.appTypes);
        // let's send a message to all server apps!
        this.sendMessage('hello from webclient example app!');
        //it's a bit sad if we left anyone out. (self.appTypes[0] contains 'all')
        this.sendMessage("hello to all!", this.appTypes[0]);  // default app type is: serverApp
    }

    onMessage(message, origin){
        console.log(message, origin);
    }

    onClose(data){
        console.log('Websocket closed with data: ', data);
    }

    onError(error){
        console.log('An error has occured when trying to connect.. ', error);
    }

    onNewPeer(peer){
        console.log('New peer connected with uid:', peer.uid, 'app type:', peer.appType);
        console.log('Lets say hello to our new friend!');
        this.sendMessage('Hello friend!', 'uid', peer.uid);
    }

    onPeerDisconnect(peer){
        console.log('Bye peer!', peer);
    }
}