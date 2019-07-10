class EmptyApp extends RpaeApp{
    constructor(){
        super('EmptyApp'); // Set your AppID here
        // once the host is set it will automatically attempt a connection to the server
        // you can set a different host, the default will be the websites url with port 9738
        //this.host = 'ws://' + window.location.hostname + ':9738';
        // If you use the default host you still have to open it calling openSocket()
        this.openSocket();
    }

    // Executes on the handshake has completed
    onOpen(data){

    }

    // Executes when a message is received
    onMessage(message, origin){

    }

    // Executes once the connection is lost.
    onClose(data){

    }

    // Executes if the connection could not be made
    onError(error){

    }

    // Executes when a new peer connects
    onNewPeer(peer){

    }

    // Executes when a peer disconnects
    onPeerDisconnect(peer){

    }
}