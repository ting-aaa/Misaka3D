#pragma once

#include <memory>
#include "../renderer/Shader.h"
#include "../resources/IResource.h"

namespace Misaka {

class MaterialBase : public IResource {
protected:
    std::shared_ptr<Shader> _shader;

public:
    static constexpr ResourceType TypeEnum = ResourceType::Material;

    MaterialBase(std::shared_ptr<Shader> shader);
    virtual ~MaterialBase() = default;

    virtual void uploadUniforms() = 0;
    
    Shader* GetShader() const;

    ResourceType GetResourceType() const override;
};

} // namespace Misaka
