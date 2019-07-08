class MediaPlayer extends RpaeApp{
    constructor(){
        super('MediaPlayer');                    // Set your AppID here
        this.host = 'ws://192.168.2.8:9738';    // Setting host will immediately start up the connection.
        //this.openSocket();
        this.playFile = null;
    }

    onOpen(data){
    }

    onMessage(message, origin){
        try{
            let msg = JSON.parse(message);
            if (msg['playing'] && msg['action'] === undefined){
                this.playFile = new IcyCast();
                this.playFile.updateData(msg['fileData']);
                let d = this.playFile.name;
                if (this.playFile.title !== '')
                    d += ' ' + this.playFile.title;
                $('#rpaePlayer-playing').text(d);
            }
            else if (!msg['playing'] && msg['action'] === undefined) $('#rpaePlayer-playing').text('');
            else if (msg['action'] === 'updateIcyData'){
                this.playFile.updateData(msg);
                let d;
                if (this.playFile.name !== '') {
                    if (this.playFile.title === '')d = this.playFile.name;
                    else d = this.playFile.name + ' - ' + this.playFile.title;
                }
                else d = this.playFile.title;
                $('#rpaePlayer-playing').text(d);
            }
        }catch (e) {

        }
        //console.log(message, origin);
    }

    onClose(data){
        console.log('Websocket in MediaPlayer closed with data: ', data);
    }

    onError(error){
        console.log('An error has occured when trying to connect.. ', error);
    }

    onNewPeer(peer){
        if (peer.appType === 'clientApp' && this.playFile != null){
            let msg = this.playFile.data;
            msg['action'] = 'sendStatus';
            this.sendMessage(JSON.stringify(msg));
        }
        console.log('New peer connected to mediaplayer with uid:', peer.uid, 'app type:', peer.appType);

    }

    onPeerDisconnect(peer){
    }

    playStream(stream, name=null){
        if (name == null)
            this.sendMessage('PLAY '+stream);
        else
            this.sendMessage('PLAY '+stream+' '+name);
    }

    stop(){
        this.sendMessage('STOP');
        $('#rpaePlayer-playing').text('');
    }
}