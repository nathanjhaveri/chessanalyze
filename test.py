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

if __name__ == '__main__':
    unittest.main()

