// Ad Maiorem Dei Gloriam!
// The Unlicense (2026).

#include <unistd.h>
#define BUFFERSIZE 256

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>

struct database {
  size_t size;
  size_t allocatedBytes;
  char *data;
};

void initDatabase(struct database* db) {
  db->size = 0;
  db->allocatedBytes = 2;

  db->data = malloc(sizeof(char) * db->allocatedBytes);
}

void add(struct database *db, char *key, char *value) {
  uint8_t keySize = strlen(key);
  uint8_t valueSize = strlen(value);
  size_t size = keySize + valueSize + 2; // 1 Byte before the data to input the size;
  
  if (db->size + size > db->allocatedBytes) {
    while (db->allocatedBytes < db->size + size) {
      db->allocatedBytes *= 2;
    }
    db->data = realloc(db->data, db->allocatedBytes);
  }

  // Setting the byte to say the key size
  db->data[db->size] = keySize;
  memcpy(db->data + db->size + 1, key, keySize);

  // Setting the byte to the value size
  db->data[db->size + 1 + keySize] = valueSize;
  memcpy(db->data + db->size + 2 + keySize, value, valueSize);

  db->size += size;
}

int get(struct database *db, char *key, char *buffer) {
  uint8_t keySize = strlen(key);
  int i = 0;
  while (i < db->size) {
    uint8_t currentKeySize = db->data[i];
    if (currentKeySize == (uint8_t)keySize) {
      if (strncmp(db->data + i + 1, key, keySize) == 0) {
        i += currentKeySize + 1; // Getting at the size of the value;
        uint8_t currentValueSize = db->data[i];
        memcpy(buffer, db->data + i, keySize);
        return 1;
      } else {
        i += currentKeySize + 1;
        i += db->data[i] + 1; // The current value Size
      }
    } else {
      i += currentKeySize + 1;
      i += db->data[i] + 1; // Jumping the entire pair
    }
  }
  return 0;
}

void rmv(struct database *db) {
  free(db->data);
}

void printDb(struct database * db) {
  char keyBuffer[BUFFERSIZE], valBuffer[BUFFERSIZE]; // To insert the null character.
  uint64_t i = 0;
  
  while (i < db->size) {
    uint8_t keySize = db->data[i];
    uint8_t valSize = db->data[i + keySize + 1];

    memcpy(keyBuffer, db->data + i + 1, keySize);
    memcpy(valBuffer, db->data + i + keySize + 2, valSize);

    keyBuffer[keySize] = '\0';
    valBuffer[valSize] = '\0';

    printf("%s %s\n", keyBuffer, valBuffer);
    i += keySize + valSize + 2; 
  }
}

int main() {
  printf("Initializing Database.");
  usleep(10000);
  printf(".");
  usleep(10000);
  printf(".\n> ");

  struct database db;
  initDatabase(&db);

  int isRunning = 1;
  char action[5], key[BUFFERSIZE], value[BUFFERSIZE];

  while (isRunning) {
    scanf("%4s", action);

    if (strcmp(action, "EXIT") == 0) {
      printf("Exiting...\n");
      isRunning = 0;
    } else {
      if (strcmp(action, "ADD") == 0) {
        scanf("%255s %255s", key, value);
        add(&db, key, value);
        printf("Added value %s associated with the key %s.\n> ", value, key);
      } else if (strcmp(action, "GET") == 0) {
        scanf("%255s", key);
        int success = get(&db, key, value);
        if (!success) {
          printf("Value %s Not Found.\n> ", key);
        } else {
          printf("%s: %s\n> ", key, value);
        }
      } else if (strcmp(action, "PRNT") == 0) {
        printf("KEY VALUE\n");
        printDb(&db);
        printf("> ");
      } else {
        printf("Error: Command not found.\n"
               "Available Commands:\n"
               "EXIT - exits;\n"
               "ADD <key> <value> - adds a key to the dictionary;\n"
               "GET <key> - returns the value associated with a key, if any;\n"
               "PRNT - prints all keys and pairs to the terminal.\n> ");
      }
    }

  }

  rmv(&db);
  return 0;
}
