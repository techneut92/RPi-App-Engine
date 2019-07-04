class ExampleApp extends RpaeApp{
    constructor(){
        super('ExampleApp'); // Set your AppID here
    }

    onOpen(){
        console.log('onopen from example app');
    }

    onMessage(message){
        console.log('onMessage from example app');
    }

    onDisconnect(info){
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