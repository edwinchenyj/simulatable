#include <EnvironmentMediator.h>
#include <ISimulatable.h>

class Environment : public ISimulatable, public EnvironmentMediator
{
private:
    /* data */
public:
    Environment(/* args */);
    ~Environment();
};

Environment::Environment(/* args */)
{
}

Environment::~Environment()
{
}
