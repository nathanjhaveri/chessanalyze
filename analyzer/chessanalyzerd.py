from SimpleXMLRPCServer import SimpleXMLRPCServer
from SimpleXMLRPCServer import SimpleXMLRPCRequestHandler

from analyze import analyze
from multiprocessing import Process

def do_analyze(pgn):
    analyze(pgn, 10000, None)

def analyze_game(sender, to, subject, body):
    Process(target=do_analyze, args=(pgn,)).start()
    return "ok"

# Restrict to a particular path.
class RequestHandler(SimpleXMLRPCRequestHandler):
    rpc_paths = ('/RPC2',)

# Create server
server = SimpleXMLRPCServer(("0.0.0.0", 80), requestHandler=RequestHandler)
server.register_introspection_functions()

server.register_function(analyze_game)

# Run the server's main loop
server.serve_forever()

