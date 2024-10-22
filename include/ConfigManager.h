#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

class ConfigManager {
  private:
    char *ssid;
    char *pass;
    ConfigManager();
  public:
    ~ConfigManager();
    static ConfigManager& getInstance();
    bool load();
    bool save();
    bool reset();
    bool isDeviceConfigured();
    void getSSID(char *buf);
    void setSSID(const char *ssid);
    void getPassword(char *buf);
    void setPassword(const char *password);
};

#endif /* CONFIGMANAGER_H */