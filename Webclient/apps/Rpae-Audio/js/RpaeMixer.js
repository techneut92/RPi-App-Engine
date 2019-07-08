class RpaeMixer{
    __channels = {};
    __joined_volume;
    __mute;
    __startup_volume;
    __volume;
    __controller;

    constructor(controller, data){
        this.__joined_volume = data['joined_volume'];
        this.__mute = data['mute'];
        this.__startup_volume = data['startup_volume'];
        this.__volume = data['volume'];
        this.__controller = controller;
        for (let key in data['channels'])
            if (data['channels'].hasOwnProperty(key))
                this.__channels[key] = new RpaeChannel(this.__controller, key, data['channels'][key]);
    }
}