#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <utility>
#include <map>

struct HttpRequest {
  std::string method, path, body;
  std::map<std::string, std::string> get_params;
};

std::pair<int, std::string> ParseIdAndComment(const std::string& body);

template <typename T>
T from_string(const std::string& s) {
  T x;
  std::istringstream is(s);
  is >> x;
  return x;
}

class CommentServer {
private:
  std::vector<std::vector<std::string>> comments_;

public:
  void ServeRequest(const HttpRequest& req, std::ostream& os) {
    if (req.method == "POST") {
      if (req.path == "/add_user") {
        comments_.emplace_back();
        auto response = std::to_string(comments_.size() - 1);
        os << "HTTP/1.1 200 OK\r\n"
           << "Content-Length: " << response.size() << "\r\n"
           << "\r\n"
           << response;
      } else if (req.path == "/add_comment") {
        auto [user_id, comment] = ParseIdAndComment(req.body);
        comments_[user_id].push_back(comment);

        os << "HTTP/1.1 200 OK\r\n\r\n";
      } else {
        os << "HTTP/1.1 404 Not found\r\n\r\n";
      }
    } else if (req.method == "GET") {
      if (req.path == "/user_comments") {
        int user_id = from_string<int>(req.get_params.at("user_id"));
        std::string response;
        for (const std::string& c : comments_[user_id]) {
          response += c + '\n';
        }

        os << "HTTP/1.1 200 OK\r\n"
           << "Content-Length: " << response.size() << "\r\n"
           << "\r\n"
           << response;
      } else {
        os << "HTTP/1.1 404 Not found\r\n\r\n";
      }
    }
  }
};

std::pair<int, std::string> ParseIdAndComment(const std::string& body) {
  int user_id;
  std::string comment;
  std::istringstream is(body);
  is >> user_id >> comment;
  return {user_id, comment};
}

int main() {
  CommentServer cs;

  const HttpRequest requests[] = {
    {"POST", "/add_user", "", {}},
    {"POST", "/add_comment", "0 Hello", {}},
    {"POST", "/add_comment", "0 LOL", {}},
    {"GET", "/user_comments", "", {{"user_id", "0"}}},
    {"GET", "/add_user", "", {}},
    {"POST", "/user_comments", "", {}}
  };

  for (const auto& req : requests) {
    cs.ServeRequest(req, std::cout);
    std::cout << "\n=================\n";
  }
  return 0;
}