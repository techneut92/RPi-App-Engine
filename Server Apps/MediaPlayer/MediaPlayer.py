from Controller import Controller


class MediaPlayer:
    controller = None

    def __init__(self):
        self.controller = Controller(self)
        # do stuff
        self.controller.start()


# The main function to start the app
if __name__ == "__main__":
    app = MediaPlayer()      # First create an object of your app
