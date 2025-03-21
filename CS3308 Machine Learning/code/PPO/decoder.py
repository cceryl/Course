from constants import RotationType


class Decoder:
    def __init__(self):
        pass

    def decode_action(self, action: list[int]) -> tuple[int, RotationType, tuple[int, int, int]]:
        """
        Decode the action space to (index, rotation, position).
        """

        return action[0], RotationType.All[action[1]], (action[2], action[3], 0)
