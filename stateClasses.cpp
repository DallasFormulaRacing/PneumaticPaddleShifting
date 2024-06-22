#include <iostream>
#include <string>

class Status {
public:
    virtual ~Status() {}
    virtual Status* onINPUT(const std::string& input) = 0;
    virtual std::string getName() const = 0;
};




class LowGear : public Status {
public:
    State* onINPUT(const std::string& event) override {
       
       /* if (input == "shift_up") {
            return new Neutral();
        }
        */
        return this;
    }

    std::string getName() const override {
        return "LowGear/1st Gear";
    }
};





class Neutral : public Status {
public:
    State* onINPUT(const std::string& input) override {
        /*if (input == "shift_up") {
            return new MidGear();
        } else if (input == "shift_down") {
            return new LowGear();
        }*/
        return this;
    }

    std::string getName() const override {
        return "Neutral";
    }
};





class MidGear : public Status {
public:
    State* onINPUT(const std::string& input) override {
        /*if (input == "shift_up") {
            return new HighGear();
        } else if (input == "shift_down") {
            return new Neutral();
        }*/
        return this;
    }

    std::string getName() const override {
        return "MidGear";
    }
};






class HighGear : public Status {
public:
    State* onINPUT(const std::string& input) override {
        /*if (input == "shift_down") {
            return new MidGear();
        }*/
        return this;
    }

    std::string getName() const override {
        return "HighGear";
    }
};
