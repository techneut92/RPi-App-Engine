class RpaeChannel{
    __name;
    __balance;
    __cid;
    __volume;
    __controller;

    constructor(controller, name, data){
        this.__name = name;
        this.__balance = data['balance'];
        this.__cid = data['cid'];
        this.__volume = data['volume'];
        this.__controller = controller
    }
}