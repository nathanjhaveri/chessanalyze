"""
    pystockfish
    ~~~~~~~~~~~~~~~

    Wraps the Stockfish chess engine

    original
    :copyright: (c) 2013 by Jarret Petrillo.
    :license: GNU General Public License, see LICENSE for more details.
"""

import subprocess

class Engine(subprocess.Popen):
    def __init__(self, stockfishpath, ponder=False, param={}):
        subprocess.Popen.__init__(self, 
            stockfishpath,
            universal_newlines=True,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,)
        self.ponder = ponder
        self.put('uci')
        self.white_to_move = True
        if not ponder:
            self.setoption('Ponder', False)

        base_param ={
        }

        base_param.update(param)
        self.param = base_param
        for name,value in base_param.items():
            self.setoption(name,value)

    def put(self, command):
        self.stdin.write(command+'\n')

    def flush(self):
        self.stdout.flush()

    def setoption(self,optionname, value):
        self.put('setoption name %s value %s'%(optionname,str(value)))
        stdout = self.isready()
        if stdout.find('No such')>=0:
            raise ValueError(stdout)

    def setposition(self, moves=[]):
        '''
        Move list is a list of moves (i.e. ['e2e4', 'e7e5', ...]) each entry as a string.  Moves must be in full algebraic notation.
        '''
        self.white_to_move = len(moves) % 2 == 0
        self.put('position startpos moves %s' % self._movelisttostr(moves))
        self.isready()

    def go_time(self, milliseconds):
        self.put('go movetime %s' % milliseconds)
        return self._parse_output()

    def go_depth(self, depth):
        self.put('go depth %s' % depth)

    def _parse_output(self):
        """
        The last 3 lines of output from stockfish contain
        the relevant information we need and look like this:
        info depth 20 seldepth 43 score cp -287 nodes 7019143 nps 1403267 time 5002 multipv 1 pv e2d4 h4g4 b2g2 g4f4 d7d6 
        info nodes 7019143 time 5002
        bestmove e2d4 ponder h4g4
        """
        score_line = ""
        time_line = ""
        bestmove_line = ""
        while not bestmove_line.startswith("bestmove"):
            line = self.stdout.readline().strip()
            score_line = time_line
            time_line = bestmove_line
            bestmove_line = line

        # bestmove
        bestmove_parts = bestmove_line.split(" ")
        bestmove = {
                'move': bestmove_parts[1],
                'ponder': bestmove_parts[3],
        }

        # time
        time_parts = time_line.split(" ")
        time = {
                'nodes': time_parts[2],
                'time' : time_parts[4],
        }

        # Score
        score_parts = score_line.split(" ")
        i = 1
        score = {}
        while i < len(score_parts):
            if score_parts[i] == 'pv':
                # Remainder of line is sf best moves
                score['next_moves'] = score_parts[i+1:]
                i = len(score_parts)
            elif score_parts[i] == 'score':
                score['unit'] = score_parts[i+1]
                evalscore = int(score_parts[i+2])
                if not self.white_to_move:
                    # If black is set to move, flip score.  Score is always from
                    # perspective of stockfish but we want positive for white, negative
                    # for black
                    evalscore = -1 * evalscore

                if score['unit'] == 'cp':
                    # Use units as pawns rather than centipawns
                    score['unit'] = 'pawns'
                    evalscore = evalscore / 100.0

                score['score'] = evalscore
                if score_parts[i+3] != "nodes":
                    score['modifier'] = score_parts[i+3]
                    i += 1
                i += 3
            else:
                score[score_parts[i]] = score_parts[i+1]
                i += 2

        return {
                'analysis': score,
                'time': time,
                'bestmove': bestmove,
        }

    def fen(self):
        self.put('d') # Stockfish debug or display?
        line = ""
        while not line.startswith('Fen'):
            line = self.stdout.readline().strip()

        fen = line[len("Fen :"):]
        return fen

    def _movelisttostr(self,moves):
        '''
        Concatenates a list of strings
        '''
        movestr = ''
        for h in moves:
            movestr += h + ' '
        return movestr.strip()

    def bestmove(self):
        last_line = ""
        self.go()
        while True:
            text = self.stdout.readline().strip()
            split_text = text.split(' ')
            if split_text[0]=='bestmove':
                return {'move': split_text[1],
                        'ponder': split_text[3],
                        'info': last_line
                        }
            last_line = text

    def isready(self):
        '''
        Used to synchronize the python engine object with the back-end engine.  Sends 'isready' and waits for 'readyok.'
        '''
        self.put('isready')
        lastline = ''
        while True:
            text = self.stdout.readline().strip()
            if text == 'readyok':
                return lastline
            lastline = text

