from SimpleXMLRPCServer import SimpleXMLRPCServer
from SimpleXMLRPCServer import SimpleXMLRPCRequestHandler

from analyze import analyze


def analyze_game(sender, to, subject, body):
    analyze(body, 10000, None)
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

