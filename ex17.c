#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "util/strlcpy.h"

struct Address {
    int id;
    int set;
    char *name;
    char *email;
};

struct Database {
    int max_rows;
    int max_data;
    struct Address rows[];
};

struct Connection {
    FILE *file;
    struct Database *db;
};

static struct Connection conn = {NULL, NULL};

void Database_close();

void die(const char *message) {
    if (errno) {
        perror(message);
    } else {
        printf("ERROR: %s\n", message);
    }

    Database_close();
    exit(1);
}

void Address_print(struct Address *addr) {
    printf("%d %s %s\n", addr->id, addr->name, addr->email);
}

void Database_load() {
    int max_data, max_rows;
    if (fread(&max_rows, sizeof(int), 1, conn.file) != 1) {
        die("Failed to load database.");
    }

    if (fread(&max_data, sizeof(int), 1, conn.file) != 1) {
        die("Failed to load database.");
    }

    // Initialise our database
    conn.db = malloc(sizeof(struct Database) + sizeof(struct Address) * max_rows);
    conn.db->max_rows = max_rows;
    conn.db->max_data = max_data;

    for (int i = 0; i < conn.db->max_rows; i++) {
        struct Address *addr = &conn.db->rows[i];
        addr->name = malloc(conn.db->max_data);
        addr->email = malloc(conn.db->max_data);

        if (fread(&addr->id, sizeof(addr->id), 1, conn.file) != 1) {
            die("Failed to load database.");
        }
        if (fread(&addr->set, sizeof(addr->set), 1, conn.file) != 1) {
            die("Failed to load database.");
        }
        if (fread(addr->name, conn.db->max_data, 1, conn.file) != 1) {
            die("Failed to load database.");
        }
        if (fread(addr->email, conn.db->max_data, 1, conn.file) != 1) {
            die("Failed to load database.");
        }
    }
}

void Database_open(const char *filename, char mode) {
    if (mode == 'c') {
        conn.file = fopen(filename, "w");
    } else {
        conn.file = fopen(filename, "r+");

        if (conn.file) {
            Database_load();
        }
    }

    if (!conn.file) die("Failed to open the file");
}

void Database_close(void) {
    if (conn.file) fclose(conn.file);
    if (conn.db) {
        for (int i = 0; i < conn.db->max_rows; i++) {
            struct Address *addr = &conn.db->rows[i];
            if (addr->name) free(addr->name);
            if (addr->email) free(addr->email);
        }
                
        free(conn.db);
    }
}

void Database_write(void) {
    rewind(conn.file);

    // Let's write max_rows and max_data first
    if (fwrite(&conn.db->max_rows, sizeof(conn.db->max_rows), 1, conn.file) != 1) {
        die("Failed to write database");
    }

    if (fwrite(&conn.db->max_data, sizeof(conn.db->max_data), 1, conn.file) != 1) {
        die("Failed to write database");
    }

    // Iterate through our aray
    for (int i = 0; i < conn.db->max_rows; i++) {
        struct Address *addr = &conn.db->rows[i];
        if (fwrite(&addr->id, sizeof(addr->id), 1, conn.file) != 1) {
            die("Failed to write database");
        }
        if (fwrite(&addr->set, sizeof(addr->set), 1, conn.file) != 1) {
            die("Failed to write database");
        }
        if (fwrite(addr->name, conn.db->max_data, 1, conn.file) != 1) {
            die("Failed to write database");
        }
        if (fwrite(addr->email, conn.db->max_data, 1, conn.file) != 1) {
            die("Failed to write database");
        }
    }

    if (fflush(conn.file) == -1) {
        die("Cannot flush database.");
    }
}

void Database_create(int max_rows, int max_data) {
    conn.db = malloc(sizeof(struct Database) + sizeof(struct Address) * max_rows);
    if (!conn.db) die("Memory error");

    conn.db->max_rows = max_rows;
    conn.db->max_data = max_data;

    for (int i = 0; i < max_rows; i++) {
        struct Address *addr = &conn.db->rows[i];
        addr->id = 1;
        addr->set = 0;
        
        addr->name = malloc(conn.db->max_data);
        if (!addr->name) die("Memory error");

        addr->email = malloc(conn.db->max_data);
        if (!addr->email) die("Memory error");
    }
}

void Database_set(int id, const char *name, const char *email) {
    struct Address *addr = &conn.db->rows[id];
    if (addr->set) die("Already set, delete it first");

    addr->set = 1;
    // Warning: only works on osx/bsd
    size_t res = strlcpy(addr->name, name, conn.db->max_data);
    if (res == 0) die ("Name copy failed");

    res = strlcpy(addr->email, email, conn.db->max_data);
    if (res == 0) die("Email copy failed");
}

void Database_get(int id) {
    struct Address *addr = &conn.db->rows[id];

    if (addr->set) {
        Address_print(addr);
    } else {
        die("ID is not set");
    }
}

void Database_delete(int id) {
    struct Address addr = {.id = id, .set = 0};

    addr.email = malloc(conn.db->max_data);
    addr.name = malloc(conn.db->max_data);
    conn.db->rows[id] = addr;
}

void Database_find(const char *string) {
    for (int i = 0; i < conn.db->max_rows; i++) {
        struct Address *addr = &conn.db->rows[i];
        int match = strcasestr(addr->name, string) || strcasestr(addr->email, string);
        if (addr->set && match) {
            Address_print(addr);
        }
    }
}

void Database_list() {
    for(int i = 0; i < conn.db->max_rows; i++) {
        struct Address *cur = &conn.db->rows[i];

        if (cur->set) {
            Address_print(cur);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3 ) die("USAGE: ex17 <dbfile> <action> [action parameters]");

    char *filename = argv[1];
    char action = argv[2][0];
    Database_open(filename, action);
    int id = 0;

    if (action == 'g' || action == 's' || action == 'd') {
        if (argc > 3) id = atoi(argv[3]);
        if (id > conn.db->max_rows) die("id too large.");
    };

    switch (action) {
        case 'c':
            if (argc != 5) die("Need max_rows and max_data"); 

            int max_rows = atoi(argv[3]);
            int max_data = atoi(argv[4]);

            Database_create(max_rows, max_data);
            Database_write();
            break;
        case 'g':
            if (argc != 4) die("Need an id to get");

            Database_get(id);
            break;
        case 's':
            if (argc != 6) die("Need id, name, email to set");

            Database_set(id, argv[4], argv[5]);
            Database_write();
            break;
        case 'd':
            if (argc != 4) die("Need id to delete");
            
            Database_delete(id);
            Database_write();
            break;
        case 'l':
            Database_list();
            break;
        case 'f':
            if (argc != 4) die("Need a search string");

            Database_find(argv[3]);
            break;
        default:
            die("Invalid action, only: c=create, g=get, s=set, d=delete, l=list");
    }

    Database_close();

    return 0;
}
