from collections import namedtuple

HttpRequest = namedtuple('HttpRequest', 'method,path,body,get_params')

class CommentServer(object):
  def __init__(self):
    self.comments = []

  def serve_request(self, req):
    if req.method == 'POST':
      if req.path == '/add_user':
        self.comments.append([])
        response = str(len(self.comments) - 1)
        return '\r\n'.join([
          'HTTP/1.1 200 OK',
          'Content-Length: {}'.format(len(response)),
          '',
          response
        ])
      elif req.path == '/add_comment':
        user_id, comment = req.body.split('\t', 1)
        self.comments[int(user_id)].append(comment)

        return 'HTTP/1.1 200 OK\r\n\r\n'
      else:
        return 'HTTP/1.1 404 Not found\r\n\r\n'
    elif req.method == 'GET':
      if req.path == '/user_comments':
        user_id = int(req.get_params['user_id'])
        response = '\n'.join(self.comments[user_id])

        return '\r\n'.join([
          'HTTP/1.1 200 OK',
          'Content-Length: {}'.format(len(response)),
          '',
          response
        ])
    else:
      return 'HTTP/1.1 404 Not found\r\n\r\n'