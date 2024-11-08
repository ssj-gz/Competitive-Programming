#include <iostream>
#include <regex>
#include <map>

#include <cassert>

using namespace std;

struct Arg
{
    enum Type {Integer, Wire};
    Type type;
    uint16_t integer = -1;
    string wire = {};
};

ostream& operator<<(ostream& os, const Arg& arg)
{
    os << "[" << (arg.type == Arg::Integer ? "Integer: " : "Wire: ") << (arg.type == Arg::Integer ? std::to_string(arg.integer): arg.wire) << "]";
    return os;
}

struct Op
{
    enum Type {DirectInput, And, LShift, RShift, Not, Or };
    Type type;

    Arg arg1 = {};
    Arg arg2 = {};
};

uint16_t calcSignalOnWire(const auto& wireName, map<string, Op>& inputOpForWire, map<string, uint16_t>& signalToWireLookup);

uint16_t calcArgValue(const Arg& arg, map<string, Op>& inputOpForWire, map<string, uint16_t>& signalToWireLookup)
{
    switch (arg.type)
    {
        case Arg::Integer:
            return arg.integer;
        case Arg::Wire:
            return calcSignalOnWire(arg.wire, inputOpForWire, signalToWireLookup);
    }
    assert(false);
    return -1;
}

uint16_t calcSignalOnWire(const auto& wireName, map<string, Op>& inputOpForWire, map<string, uint16_t>& signalToWireLookup)
{
    std::cout << " calcSignalOnWire: " << wireName << std::endl;
    assert(inputOpForWire.contains(wireName));
    if (signalToWireLookup.contains(wireName))
        return signalToWireLookup[wireName];
    const auto& inputOp = inputOpForWire[wireName];
    auto calcSignal = [&inputOpForWire, &inputOp, &signalToWireLookup]() -> uint16_t
    {
        switch (inputOp.type)
        {
            case Op::DirectInput:
                return calcArgValue(inputOp.arg1, inputOpForWire, signalToWireLookup);
            case Op::And:
                return calcArgValue(inputOp.arg1, inputOpForWire, signalToWireLookup) & calcArgValue(inputOp.arg2, inputOpForWire, signalToWireLookup);
            case Op::LShift:
                return calcArgValue(inputOp.arg1, inputOpForWire, signalToWireLookup) << calcArgValue(inputOp.arg2, inputOpForWire, signalToWireLookup);
            case Op::RShift:
                return calcArgValue(inputOp.arg1, inputOpForWire, signalToWireLookup) >> calcArgValue(inputOp.arg2, inputOpForWire, signalToWireLookup);
            case Op::Not:
                return ~calcArgValue(inputOp.arg1, inputOpForWire, signalToWireLookup);
            case Op::Or:
                return calcArgValue(inputOp.arg1, inputOpForWire, signalToWireLookup) | calcArgValue(inputOp.arg2, inputOpForWire, signalToWireLookup);
        };
        assert(false);
        return -1;
    };

    const auto signal = calcSignal();
    signalToWireLookup[wireName] = signal;

    return signal;
};

int main()
{
    std::regex connectionRegex(R"(^(((\w+)|((\w+) AND (\w+))|((\w+) LSHIFT (\d+))|(NOT (\w+))|((\w+) OR (\w+))|((\w+) RSHIFT (\d+)))) -> (\w+)$)");
    string connectionLine;
    map<string, Op> inputOpForWire;
    while (getline(cin, connectionLine))
    {
        std::cout << "connectionLine: " << connectionLine << std::endl;
        std::smatch connectionMatch;
        const bool matchSuccessful = std::regex_match(connectionLine, connectionMatch, connectionRegex);
        assert(matchSuccessful);

        auto parseArg = [](const std::string& argString)
        {
            Arg arg;
            try
            {
                arg.integer = std::stoi(argString);
                arg.type = Arg::Integer;
            }
            catch (std::invalid_argument const& ex)
            {
                assert(!argString.empty());
                arg.type = Arg::Wire;
                arg.wire = argString;
            }
            return arg;
        };

        Op op;
        if (!connectionMatch.str(3).empty())
        {
            // No operation (direct input).
            op.type = Op::DirectInput;
            op.arg1 = parseArg(connectionMatch.str(3));
            std::cout << "Direct input from " << op.arg1 << std::endl;
        }
        else if (!connectionMatch.str(4).empty())
        {
            // AND.
            op.type = Op::And;
            op.arg1 = parseArg(connectionMatch.str(5));
            op.arg2 = parseArg(connectionMatch.str(6));
            std::cout << "AND of " << op.arg1 << " and " << op.arg2 << std::endl;
        }
        else if (!connectionMatch.str(7).empty())
        {
            // LSHIFT.
            op.type = Op::LShift;
            op.arg1 = parseArg(connectionMatch.str(8));
            op.arg2 = parseArg(connectionMatch.str(9));
            std::cout << "LSHIFT of " << op.arg1 << " and " << op.arg2 << std::endl;
        }
        else if (!connectionMatch.str(10).empty())
        {
            // NOT.
            op.type = Op::Not;
            op.arg1 = parseArg(connectionMatch.str(11));
            std::cout << "NOT of " << op.arg1 << std::endl;
        }
        else if (!connectionMatch.str(12).empty())
        {
            // OR.
            op.type = Op::Or;
            op.arg1 = parseArg(connectionMatch.str(13));
            op.arg2 = parseArg(connectionMatch.str(14));
            std::cout << "OR of " << op.arg1 << " and " << op.arg2 << std::endl;
        }
        else if (!connectionMatch.str(15).empty())
        {
            // RSHIFT.
            op.type = Op::RShift;
            op.arg1 = parseArg(connectionMatch.str(16));
            op.arg2 = parseArg(connectionMatch.str(17));
            std::cout << "RSHIFT of " << op.arg1 << " and " << op.arg2 << std::endl;
        }
        else 
        {
            assert(false);
        }
        const auto recipientWire = connectionMatch.str(18);
        std::cout << " recipient:" << recipientWire << std::endl;
        assert(!recipientWire.empty());
        if (inputOpForWire.contains(recipientWire))
        {
            assert(false);
        }
        inputOpForWire[recipientWire] = op;
    }

    std::map<string, uint16_t> signalToWireLookup;
    const auto origSignalToA = calcSignalOnWire("a", inputOpForWire, signalToWireLookup);
    std::cout << "original signal to a : " << origSignalToA << std::endl;
    signalToWireLookup.clear();
    inputOpForWire["b"] = {Op::DirectInput, {Arg::Integer, origSignalToA}};
    const auto finalSignalToA = calcSignalOnWire("a", inputOpForWire, signalToWireLookup);
    std::cout << "final signal to a : " << finalSignalToA << std::endl;

    return 0;
}
