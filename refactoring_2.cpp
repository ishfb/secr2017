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

enum class HttpCode {
  OK = 200,
  NotFound = 404,
};

class HttpResponse {
public:
  explicit HttpResponse(HttpCode code);

  HttpResponse& SetContent(std::string value);

  friend std::ostream& operator << (std::ostream& os, const HttpResponse& resp);
};

HttpResponse::HttpResponse(HttpCode code) {
}

HttpResponse& HttpResponse::SetContent(std::string value) {
}

std::ostream& operator << (std::ostream& os, const HttpResponse& resp) {
  return os;
}

class CommentServer {
private:
  std::vector<std::vector<std::string>> comments_;

public:
  void ServeRequest(const HttpRequest& req, std::ostream& os) {
    if (req.method == "POST") {
      if (req.path == "/add_user") {
        comments_.emplace_back();

        os << HttpResponse(HttpCode::OK).SetContent(std::to_string(comments_.size() - 1));
      } else if (req.path == "/add_comment") {
        auto [user_id, comment] = ParseIdAndComment(req.body);
        comments_[user_id].push_back(comment);

        os << HttpResponse(HttpCode::OK);
      } else {
        os << HttpResponse(HttpCode::NotFound);
      }
    } else if (req.method == "GET") {
      if (req.path == "/user_comments") {
        int user_id = from_string<int>(req.get_params.at("user_id"));
        std::string response;
        for (const std::string& c : comments_[user_id]) {
          response += c + '\n';
        }

        os << HttpResponse(HttpCode::OK).SetContent(response);
      } else {
        os << HttpResponse(HttpCode::NotFound);
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