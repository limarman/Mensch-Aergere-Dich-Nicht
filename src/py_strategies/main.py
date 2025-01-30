from typing import Dict, List

# YOUR CODE

def get_move(position: Dict, dieroll: int, allowed_indices: List[int], successor_positions: List[Dict]):
    """
    Chooses the next move given a position. Each position object is given as a dictionary consisting of a list
    containing the positions of both player's pieces and the turn indicator, e.g.:
    {
      'player0': [-1, -1, 6, 24],
      'player1': [-1, -1, 5, 42],
      'turn': False
    }
    The numeric value of the turn indicator describes the player to move, e.g. turn = False -> turn = 0 -> player0's move.
    In this challenge you will always be queried as player0 (player with the first move advantage), i.e. for the current position it will always hold: turn = False
    The piece position list is always given sorted in ascending order.
    Value -1 corresponds to the starting box (B-squares), values 40, 41, 42, 43 are the finish squares.
    All the other piece positions (in [0, 39]) are given from the view point of the corresponding player, e.g. 0 - starting square (A-square).
    To toggle between the different views, you can use the helper function "toggle_piece_space" below.

    To illustrate how this function should be implemented see the following example:

    Example Input:
        position = {'player0': [-1, -1, 6, 24], 'player1': [-1, -1, 5, 42], 'turn': False}
        dieroll = 1
        allowed_indices = [2, 3]
        successor_positions = [
           {'player0': [-1, -1, 7, 24], 'player1': [-1, -1, 5, 42], 'turn': True},
           {'player0': [-1, -1, 6, 25], 'player1': [-1, -1, -1, 42], 'turn': True}
        ]

    In the current position (position) you have two pieces outside your starting box - on squares 6 and 24.
    With the die having rolled 1, you have two options:
        Moving the piece on square 6 (index 2 in position['player0']) or the piece on square 24 (index 3 in position['player0'])
        Thus: allowed_indices = [2, 3]
    If moving the piece with index 2, you bring it from square 6 to 7. In the resulting position it is the opponent's turn to move.
        -> successor_positions[0] = {'player0': [-1, -1, 7, 24], 'player1': [-1, -1, 5, 42], 'turn': True}
    If moving the piece with index 3, you bring it from square 24 to 25. This captures an opponent's piece (piece position 5 from opponent's view)
        -> successor_positions[1] = {'player0': [-1, -1, 6, 25], 'player1': [-1, -1, -1, 42], 'turn': True}
    
    Let's assume you decide that the 2nd option is better. You return allowed_indices[1] = 3.
    
    Args:
        position: Dictionary describing the current position
        dieroll: Your turn's die roll [1-6]
        allowed_indices: A list of the possible pieces that can be picked for the next move.
                         The pieces are indicated with their index in the piece position list of the player to move.
                         This is a sublist of [0, 1, 2, 3].
        successor_positions: A list of the successor positions corresponding to the possible piece indices in allowed_indices.
                             Has always the same length as allowed_indices.

    Returns:
        one index from allowed_indices indicating which piece to pick for the next move.
    """

    # OVERWRITE THIS CODE FOR YOUR STRATEGY
    # Exemplary implementation of Furthest Piece or Capture (FPoC)

    # Figure out which Player is queried
    # Note that this is not strictly necessary, you will always be queried as player0
    player_to_move = 'player' + str(int(position['turn']))
    opponent_player = 'player' + str(int(not position['turn']))
                              
    for piece_index in allowed_indices[::-1]:
        next_pos = position[player_to_move][piece_index] + dieroll

        # check whether this is a capturing move
        if next_pos != -1 and next_pos < 40 and toggle_piece_space([next_pos])[0] in position[opponent_player]:
            return piece_index

    # if no capture possible, return the highest index
    return allowed_indices[-1]


def toggle_piece_space(pieces):
    """
    Helper function that toggles the view on the pieces between players.
    E.g. Starting square from view of one player has index 0, from the other player index 20.
    Performs the mapping: 0 -> 20, 1 -> 21, ... 19 -> 39, 20 -> 0, 21 -> 1, ... 39 -> 19
    Note that the starting squares (-1) and finish squares (40, 41, 42, 43) are not toggled as they are disjunct between players.

    Example:
    pieces = [-1, 0, 21, 40]
    toggled_pieces = toggle_piece_space(pieces)
    print(toggled_pieces)
    >> [-1, 20, 1, 40]

    Args:
        pieces: list of piece indices from one player's view.
    Returns:
        list of piece indices from the other player's view.
    """
    toggled_pieces = []
    for piece in pieces:
        if piece == -1 or piece >= 40:
            toggled_pieces.append(piece)
        else:
            toggled_piece = (piece + 20) % 40
            toggled_pieces.append(toggled_piece)

    return toggled_pieces

