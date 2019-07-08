class MediaPlayer extends RpaeApp{
    constructor(){
        super('MediaPlayer');                    // Set your AppID here
        this.host = 'ws://192.168.2.8:9738';    // Setting host will immediately start up the connection.
        this.playFile = null;
    }

    onOpen(data){
        console.log('mediaplayer is active', data);
    }

    onMessage(message, origin){
        try{
            let msg = JSON.parse(message);
            if (msg['action'] === 'sendStatus'){
                this.playFile = new IcyCast();
                this.playFile.updateData(msg);
            }
            else if (msg['action'] === 'updateIcyData'){
                this.playFile.updateData(msg);
            }
        }catch (e) {

        }
        console.log(message, origin);
    }

    onClose(data){
        console.log('Websocket in MediaPlayer closed with data: ', data);
    }

    onError(error){
        console.log('An error has occured when trying to connect.. ', error);
    }

    onNewPeer(peer){
        console.log('New peer connected to mediaplayer with uid:', peer.uid, 'app type:', peer.appType);

    }

    onPeerDisconnect(peer){
        console.log('Bye peer!', peer);
    }

    playStream(stream){
        this.sendMessage('PLAY '+stream);
    }

    stop(){
        this.sendMessage('STOP');
    }
}