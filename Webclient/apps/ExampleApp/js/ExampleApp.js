class ExampleApp extends RpaeApp{
    constructor(){
        super('ExampleApp'); // Set your AppID here
        this.host = 'ws://192.168.2.8:9738';
    }

    onOpen(){
        console.log('onopen from example app');
        this.sendMessage('hello from webclient example app!');
    }

    onMessage(message){
        console.log('onMessage from example app');
    }

    onClose(data){
        console.log('onDisconnect from example app');
    }

    onError(error){
        console.log('onError from example app');
    }

    onNewPeer(peer){
        console.log('onNewPeer from example app');
    }

    onPeerDisconnect(peer){
        console.log('onPeerDisconnect from example app');
    }
}