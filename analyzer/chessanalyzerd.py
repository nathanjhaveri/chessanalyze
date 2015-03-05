from SimpleXMLRPCServer import SimpleXMLRPCServer
from SimpleXMLRPCServer import SimpleXMLRPCRequestHandler

def analyze_game(data):
    print "analyzing game"
    print data
    return "done"

# Restrict to a particular path.
class RequestHandler(SimpleXMLRPCRequestHandler):
    rpc_paths = ('/RPC2',)

# Create server
server = SimpleXMLRPCServer(("0.0.0.0", 80), requestHandler=RequestHandler)
server.register_introspection_functions()

server.register_function(analyze_game)

# Run the server's main loop
server.serve_forever()

