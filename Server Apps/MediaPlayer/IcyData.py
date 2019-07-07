import requests
from threading import Thread


class IcyData:
    __name = None
    __title = ''
    __description = None
    __url = None
    __genre = None
    __bitrate = None
    __metaint = None
    __requestUrl = None
    __encoding = 'latin1'
    __headers = None
    __updateThread = None
    __onUpdate = None
    __close = False

    def __init__(self, r_url, onUpdate):
        self.__requestUrl = r_url
        self.__detectEncoding()
        self.__updateThread = Thread(target=self.__requestData)
        self.__updateThread.start()
        self.__onUpdate = onUpdate

    def __del__(self):
        self.__close = True

    def __detectEncoding(self):
        r_url = self.__requestUrl.lower()
        if 'mp3' in r_url:
            self.__encoding = 'iso-8859-1'
        elif 'ogg' in r_url:
            self.__encoding = 'utf-8'

    def __requestData(self):
        response = requests.get(self.__requestUrl, headers={'Icy-MetaData': '1'}, stream=True)
        self.__headers, stream = response.headers, response.raw
        self.__metaint = int(self.__headers['icy-metaint'])
        self.__name = self.__headers['icy-name']
        self.__description = self.__headers['icy-description']
        self.__url = self.__headers['icy-url']
        self.__genre = self.__headers['icy-genre']
        self.__bitrate = self.__headers['icy-br']
        audio_length = self.__metaint
        self.__onUpdate(self.data)
        while not self.__close:
            audio_data = stream.read(audio_length)  # not being used atm
            meta_byte = stream.read(1)
            if meta_byte:
                meta_length = ord(meta_byte) * 16
                meta_data = stream.read(meta_length).decode(self.__encoding).split(';')
                self.__updateData(meta_data)

    def __updateData(self, data):
        for d in data:
            if '=' in d:
                new_data = d.split('=')
                new_data[1] = new_data[1].strip('\"')
                new_data[1] = new_data[1].strip('\'').strip()
                if new_data[0] == 'StreamTitle':
                    self.__title = new_data[1]
                    self.__onUpdate(self.data)
                else:
                    print('unknown data type in icy cast:', new_data[0])

    def close(self):
        self.__close = True
        self.__updateThread.join(1)

    def start(self):
        self.__close = False
        self.__updateThread.start()

    @property
    def name(self):
        return self.__name

    @property
    def title(self):
        return self.__title

    @property
    def url(self):
        return self.__url

    @property
    def genre(self):
        return self.__genre

    @property
    def bitrate(self):
        return self.__bitrate

    @property
    def metaint(self):
        return self.__metaint

    @property
    def requestUrl(self):
        return self.__requestUrl

    @property
    def encoding(self):
        return self.__encoding

    @property
    def headers(self):
        return self.__headers

    @property
    def data(self):
        data = {
            'name': self.__name,
            'title': self.__title,
            'description': self.__description,
            'url': self.__url,
            'genre': self.__genre,
            'bitrate': self.__bitrate,
            'metaint': self.__metaint,
            'requestUrl': self.__requestUrl,
            'encoding': self.__encoding,
            # 'headers': self.__headers,
        }
        return data

    @property
    def closed(self):
        return self.__close
