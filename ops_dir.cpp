/*
 *  Copyright 2009 Michael Stephens
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <mongo/bson/bson.h>
#include <mongo/client/gridfs.h>

#include "operations.h"
#include "options.h"

int gridfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
  if (strcmp(path, "/") != 0)
    return -ENOENT;

  /* Create string to hold last filename seen */
  std::string lastFN;

  filler(buf, ".", NULL, 0);
  filler(buf, "..", NULL, 0);

  auto sdc = make_ScopedDbConnection();
  mongo::GridFS gf = get_gridfs(sdc);

  std::unique_ptr<mongo::DBClientCursor> cursor = gf.list();
  while (cursor->more()) {
    mongo::BSONObj f = cursor->next();

    /* If this filename matches the last filename we've seen, *do not* add it to the buffer because it's a duplicate filename */ 
    if (lastFN != f.getStringField("filename")) {
      filler(buf, f.getStringField("filename") , NULL , 0);
    }

    /* Update lastFN with our cursor's current filename */
    lastFN = f.getStringField("filename");
    fprintf(stderr, "DEBUG: %s\n", lastFN.c_str());
  }

  for (auto i : open_files) {
    filler(buf, i.first.c_str(), NULL, 0);
  }

  return 0;
}

