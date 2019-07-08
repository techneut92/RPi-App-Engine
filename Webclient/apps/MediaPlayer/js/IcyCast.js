class IcyCast{
    constructor() {
        this.__name = '';
        this.__url = '';
        this.__genre = '';
        this.__title = '';
        this.__metaint = '';
        this.__bitrate = 0;
        this.__description = '';
    }

    updateData(data){
        this.__name = data['name'];
        this.__url = data['url'];
        this.__genre = data['genre'];
        this.__title = data['title'];
        this.__metaint = data['metaint'];
        this.__bitrate = data['bitrate'];
        this.__description = data['description'];
    }

    get name() { return this.__name; }
    get url() { return this.__url }
    get genre() { return this.__genre; }
    get title() { return this.__title; }
    get metaint() { return this.__metaint; }
    get bitrate() { return this.__bitrate; }
    get description() { return this.__description; }
}