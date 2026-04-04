#include "file_monitor.hpp"

#include <sys/inotify.h>
#include <unistd.h>
#include <errno.h>

#include <vector>
#include <cstring>

FileMonitor::FileMonitor() = default;

FileMonitor::~FileMonitor() {
  if (wd_ >= 0 && fd_ >= 0) {
    inotify_rm_watch(fd_, wd_);
  }
  if (fd_ >= 0) {
    close(fd_);
  }
}

bool FileMonitor::init(const std::string& path) {
  watched_path_ = path;

  fd_ = inotify_init1(IN_NONBLOCK);
  if (fd_ < 0) {
    return false;
  }

  wd_ = inotify_add_watch(fd_, watched_path_.c_str(),
	IN_CREATE | IN_MODIFY | IN_CLOSE_WRITE | IN_DELETE | IN_MOVED_TO);

  if (wd_ < 0) {
    close(fd_);
    fd_ = -1;
    return false;
  }
  return true;
}

const std::string& FileMonitor::watched_path() const {
  return watched_path_;
}

std::vector<FileEvent> FileMonitor::poll_events(){
  std::vector<FileEvent> out;

  if (fd_ < 0) {
    return out;
  }

  std::vector<char> buffer(4096);
  while(true) {
    const ssize_t len = read(fd_, buffer.data(), buffer.size());
    if (len < 0) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
	break;
      }
      break;
    }

    if (len == 0) {
      break;
    }

    ssize_t i = 0;
    while ( i < len) {
      auto* ev = reinterpret_cast<struct inotify_event*>(buffer.data() + i);

      if (ev->len > 0) {
	FileEvent fe;
	fe.mask = ev->mask;
	fe.watched_path = watched_path_;
	fe.relative_path = ev->name;
	fe.full_path = watched_path_ + "/" + fe.relative_path;

	if (ev->mask & (IN_CREATE | IN_MOVED_TO)) {
	  fe.event_type = "file_created";
	  out.push_back(fe);
	} else if (ev->mask & (IN_MODIFY | IN_CLOSE_WRITE)) {
	  fe.event_type = "file_modified";
	  out.push_back(fe);
	} else if (ev->mask & IN_DELETE) {
	  fe.event_type = "file_deleted";
	  out.push_back(fe);
	}
      }

      i += sizeof(struct inotify_event) + ev->len;
    }
  }
  return out;
}
