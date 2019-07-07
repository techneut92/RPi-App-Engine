from __future__ import print_function
import re
import struct
try:
    import urllib2
except ImportError:  # Python 3
    import urllib.request as urllib2
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

    def __init__(self, r_url):
        self.__requestUrl = r_url
        self.__detectEncoding()
        self.__updateThread = Thread(target=self.__requestData)
        self.__updateThread.start()

    def __detectEncoding(self):
        r_url = self.__requestUrl.lower()
        if 'mp3' in r_url:
            self.__encoding = 'iso-8859-1'
        elif 'ogg' in r_url:
            self.__encoding = 'utf-8'

    def __requestData(self):
        response = requests.get(self.__requestUrl, headers={'Icy-MetaData': '1'}, stream=True)
        self.__headers, stream = response.headers, response.raw
        print(self.__headers)
        self.__metaint = int(self.__headers.['icy-metaint'])
        self.__name = self.__headers.get['icy-name']
        self.__description = self.__headers['icy-description']
        self.__url = self.__headers['icy-url']
        self.__genre = self.__headers['icy-genre']
        self.__bitrate = self.__headers['icy-br']
        audio_length = self.__metaint
        self.__fillData()
        while True:
            # new request
            # response = requests.get(self.__requestUrl, headers={'Icy-MetaData': '1'}, stream=True)
            # response.raise_for_status()
            audio_data = stream.read(audio_length)  # not being used atm
            meta_byte = stream.read(1)
            if meta_byte:
                meta_length = ord(meta_byte) * 16
                meta_data = stream.read(meta_length).decode(self.__encoding).split(';')
                self.__updateData(meta_data)

    def __updateData(self, data):
        print(data)

    def __fillData(self):
        #request = urllib2.Request(self.__requestUrl, headers={'Icy-MetaData': 1})  # request metadata
        #response = urllib2.urlopen(request)
        #self.__headers = response.headers
        #self.__metaint = int(response.headers['icy-metaint'])
        #for _ in range(10):  # title may be empty initially, try several times
        #    response.read(self.__metaint)  # skip to metadata
        #    metadata_length = struct.unpack('B', response.read(1))[0] * 16  # length byte
        #    metadata = response.read(metadata_length).rstrip(b'\0')
        #    # extract title from the metadata
        #    m = re.search(br"StreamTitle='([^']*)';", metadata)
        #    if m:
        #        title = m.group(1)
        #        if title:
        #            self.__title = title.decode(self.__encoding, errors='replace')
        #            break

        self.__name = self.__headers.get('icy-name')
        self.__description = self.__headers['icy-description']
        self.__url = self.__headers['icy-url']
        self.__genre = self.__headers['icy-genre']
        self.__bitrate = self.__headers['icy-br']

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
