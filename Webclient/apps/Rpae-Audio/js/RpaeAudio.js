class RpaeAudio extends RpaeApp{
    __mute;
    __resetVolumeAtStart;
    __startupVolume;
    __volume;
    __mixers = {};

    constructor(){
        super('Rpae-Audio');                    // Set your AppID here
        this.host = 'ws://localhost:9738';    // Setting host will immediately start up the connection.
    }

    onOpen(data){
        console.log(this.name, 'ready to process', data);
    }

    onMessage(message){
        message = JSON.parse(message);
        if (message['task'] === 'init')
            this.init(message);
        else if (message['task'] === 'update'){
            console.log('update', message['value'])
        }
    }

    init(data){
        this.__mute = data['mute'];
        this.__resetVolumeAtStart = data['reset_volume_at_start'];
        this.__startupVolume = data['startup_volume'];
        this.__volume = data['volume'];
        for (let key in data['mixers'])
            if (data['mixers'].hasOwnProperty(key))
                this.__mixers[key] = new RpaeMixer(this, data['mixers'][key]);
    }

    get volume() { return this.__volume; }
    set volume(val) {
        val = parseInt(val);
        if (val <= 100 && val >= 0){
            this.__volume = val;
            this.sendMessage(JSON.stringify({
                'task': 'setVolume',
                'value': val,
                'mixer': 'default'
            }))
        }
    }
}