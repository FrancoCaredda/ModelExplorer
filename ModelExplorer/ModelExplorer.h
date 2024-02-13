#ifndef _MODEL_EXPLORER_H
#define _MODEL_EXPLORER_H

#include "Core/AApplication.h"

class ModelExplorer : public AApplication
{
public:
	ModelExplorer(const std::string& name, uint32_t version)
		: AApplication(name, version) {}
	~ModelExplorer() = default;
private:

};

#endif // !_MODEL_EXPLORER_H
