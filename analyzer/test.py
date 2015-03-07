import unittest

from emailparse import parse_email
class TestEmailParse(unittest.TestCase):
    def setUp(self):
        f = open("pgnemail.txt", "r")
        self.raw = f.read()
        f.close()

    def testParse(self):
        message = parse_email(self.raw)
        self.assertEqual(message.sender, "nathan.jhaveri@gmail.com")
        self.assertEqual(message.name, "Nathan Jhaveri")
        self.assertEqual(message.subject, "Game against: apete")

        f = open("pgn-files/apete.pgn", "r")
        gametext = f.read()
        f.close()

        self.assertEqual(gametext, message.body)

from pystockfish import Engine
class TestEngine(unittest.TestCase):
    def setUp(self):
        class Mock(Engine):
            def __init__(self):
                self.white_to_move = True

        self.mock_eng = Mock()
        self.mock_eng.stdout = Mock()
 
    def testParseOutput(self):
        def mockread():
            yield "info depth 120 seldepth 2 score mate 1 nodes 4697 nps 391416 time 12 multipv 1 pv f7d7"
            yield "info nodes 0 time 2"
            yield "bestmove (none) ponder (none)"

        gen = mockread()
        self.mock_eng.stdout.readline = lambda: next(gen)

        out = Engine._parse_output(self.mock_eng)
        self.assertEqual(out['analysis']['score'], 1)
        self.assertEqual(out['analysis']['unit'], 'mate')

    def testParseMate(self):
        def mockread():
            yield "info depth 0 score mate 0"
            yield "info nodes 0 time 2"
            yield "bestmove (none) ponder (none)"

        gen = mockread()
        self.mock_eng.stdout.readline = lambda: next(gen)

        out = Engine._parse_output(self.mock_eng)
        self.assertEqual(out['analysis']['score'], 0)
        self.assertEqual(out['analysis']['unit'], 'mate')

    def testParseUpperbound(self):
        def mockread():
            yield "info depth 10 seldepth 12 score cp -31 upperbound nodes 66588 nps 812048 time 82 multipv 1 pv a8e8 e2e3 f5c2 d4f4 c2e4"
            yield "info nodes 66588 time 82"
            yield "bestmove a8e8 ponder e2e3"

        gen = mockread()
        self.mock_eng.stdout.readline = lambda: next(gen)

        out = Engine._parse_output(self.mock_eng)
        self.assertEqual(out['analysis']['score'], -0.31)
        self.assertEqual(out['analysis']['unit'], 'p')
        self.assertEqual(out['analysis']['modifier'], 'upperbound')


if __name__ == '__main__':
    unittest.main()

