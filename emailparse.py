import email
from email import header # TODO: Unit test failing without this, not sure why

class BasicMessage:
    pass

def parse_email(raw_msg):
    msg = email.message_from_string(raw_msg)

    message = BasicMessage()

    message.name, message.sender = email.utils.parseaddr(msg['From'])
    message.subject = get_subject(msg)
    message.body = get_message_body(msg)

    return message

def get_subject(msg):
    subject_header = msg['subject']
    decoded_list = email.header.decode_header(subject_header)
    encoded = [text.decode(encoding) if encoding else text for text, encoding in decoded_list]
    subject = u''.join(encoded)
    trimmed_subject = subject.strip()

    return trimmed_subject

def get_message_body(msg):
    # set msg to the html message
    if msg.is_multipart():
        submsgs = msg.get_payload()
        msg = submsgs[0] # Init to something (prob text/plain)
        for submsg in submsgs:
            if submsg.get_content_subtype() == 'html':
                msg = submsg

    # Need decode quoted-printable
    unquoted = msg.get_payload(decode=True)

    # Decode charset (e.g. iso-8859-1)
    body = unquoted.decode(msg.get_content_charset())

    return body


