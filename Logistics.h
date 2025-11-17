#pragma once
#ifndef LOGISTICS_H
#define LOGISTICS_H

#include <string>
#include <ostream>

class Logistics {
protected:
    std::string id_;
    std::string dest_;

public:
    Logistics(const std::string& id = "", const std::string& dest = "")
        : id_(id), dest_(dest) {
    }
    virtual ~Logistics() = default;

    const std::string& getId()  const { return id_; }
    const std::string& getDest() const { return dest_; }
    void setId(const std::string& v) { id_ = v; }
    void setDest(const std::string& v) { dest_ = v; }

    virtual int  getTimestamp() const = 0;            
    virtual void setTimestamp(int v) = 0;
    virtual const char* timestampLabel() const = 0;   

    virtual std::string toString() const = 0;
};

#endif 
