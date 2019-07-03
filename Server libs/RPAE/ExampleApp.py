from RpaeApp import RpaeApp


class ExampleApp(RpaeApp):
    def __init__(self):
        super().__init__(aid="exampleApp", host="ws://192.168.2.8:9738")  # By default the host is "ws://localhost:9738"

    def onMessage(self, message, origin):
        print(message)

    # If there is an error it will already be printed so add any other stuff you want to do here
    def onError(self, error):
        pass

    def onOpen(self):
        print("did onopen from main class")


if __name__ == "__main__":
    app = ExampleApp()
    ExampleApp().start()
