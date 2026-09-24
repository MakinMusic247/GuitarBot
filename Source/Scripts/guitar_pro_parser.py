from os import path
import guitarpro
import argparse
import json
from enum import Enum


'''
Servo class that defines:
    * what PCA9685 channel it is connected to (0-16)
    * what note/s it plays (string and fret/s)
'''
class Servo:
    board: int = 0          # board index of PCA9685 (if using mutiple)
    channel: int            # channel of PCA9685 (0-16) 
    fret: int               # guitar fret (-1 if open string)
    string_R: int           # guitar string on right side of servo horn
    string_L: int           # guitar string on left side of servo horn
    preffered_string: int   # preferred string to be played if both are requested (string_R or string_L)
    # def __init__(self, channel: int, fret: int, string_R: int, string_L: int, preffered_string: int):
    #     print("Servo:", f"channel {channel}", f"fret {fret}", f"string (Right): {string_R}", f"string (Left): {string_L}")
    def __init__(self):
        print("Servo:", f"channel {self.channel}")
        print(f"fret {self.fret}", f"string (Right): {self.string_R}", f"string (Left): {self.string_L}", f"preffered: {self.preffered_string}")



'''
Sets which side of the servo motor is pressed to select a guitar string
'''
class Side(Enum):
    RIGHT = 0
    LEFT = 1


'''
Create a map of the servo motors on GuitarBot (corresponding to string and frets)

'''
# servo_map_fretboard = [
#     []
# ]

# guitar_strings = [ [0,"E"], [1,"A"], [2,"D"], [3,"G"], [4,"B"], [5,"E"] ]

servos = [
    [
        Servo(0, 1, 0, 1), # channel 0 | fret 1 | low E + A string 
        Servo(1, 1, 2, 3), # channel 1 | fret 1 | D + G string
        Servo(2, 1, 4, 5), 
    ]
]

servo_dict = {
    0: [
        Servo(0, 1, 0, 1), # channel 0 | fret 1 | low E + A string 
        Servo(1, 1, 2, 3), # channel 1 | fret 1 | D + G string
        Servo(2, 1, 4, 5)
    ],
    2: [
        Servo(3, 2, 0, 1), # channel 4 | fret 2 | low E + A string 
        Servo(4, 2, 2, 3), 
        Servo(5, 2, 4, 5)
    ],
    3: [
        Servo(6, 3, 0, 1), # channel 6 | fret 3 | low E + A string 
        Servo(7, 3, 2, 3), 
        Servo(8, 3, 4, 5)
    ],
}


fretboard_notes = [
    [ 'E', 'A', 'D', 'G', 'B', 'E' ],       # open strings
    [ 'F', 'A#', 'D#', 'G#', 'C', 'F' ],    # fret 1
    [ 'F#', 'B', 'E', 'A', 'C#', 'F#' ],    # fret 2
    [ 'G', 'C', 'F', 'A#', 'D', 'G' ],      # fret 3
]


'''
Build a map of the servo positions on the guitar
If a note is not covered by a servo - 
'''
def build_servo_position_map(servos: list[Servo])



'''
Get the corresponding servo giving a string and fret value
'''
def get_servo(string: int, fret: int):
    print(f"Getting servo @ string {string}, fret {fret}...")
    





def read_track(track: guitarpro.Track):
    print("Reading track", track.name, "...")
    for m, measure in enumerate(track.measures):
        print(f"measure {m}", measure)
        for voice in measure.voices:
            for beat in voice.beats:
                for note in beat.notes:
                    print("note:", note.string, note.value)





def main(source, tracks):
    if tracks is None:
        tracks = ['*']
    source = 'guitarbot_test_tab.gp5'
    song = guitarpro.parse(source)
    print(song.title.title, song.tracks, song.artist, song.key) #, song.music.format)
    for track in song.tracks:
        print(track.name, track.channel, track.fretCount, track)
        read_track(track)





if __name__ == '__main__':
    import argparse

    def bitarray(string):
        return int(string, base=2)

    def tracknumber(string):
        if string == '*':
            return string
        else:
            return int(string)

    description = """
        Transpose tracks of GP tab by N semitones.
        Multiple '--track' and '--by' arguments can be specified.
        """
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('source',
                        metavar='SOURCE',
                        help='path to the source tab')
    # parser.add_argument('dest',
    #                     metavar='DEST', nargs='?',
    #                     help='path to the processed tab')
    parser.add_argument('-t', '--track',
                        metavar='NUMBER', type=tracknumber, dest='tracks',
                        action='append',
                        help='number of the track to transpose')
    # parser.add_argument('-b', '--by',
    #                     metavar='N', type=int, required=True, dest='semitones',
    #                     action='append',
    #                     help='transpose by N steps')
    # parser.add_argument('-s', '--stringmap',
    #                     metavar='BITARRAY', type=bitarray, dest='stringmaps',
    #                     action='append',
    #                     help='bit array where ones represent strings that '
    #                          'should be transposed, e.g. `100000` will '
    #                          'transpose only 6th string')
    args = parser.parse_args()
    kwargs = dict(args._get_kwargs())
    main(**kwargs)