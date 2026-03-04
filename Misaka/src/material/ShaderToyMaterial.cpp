#include "ShaderToyMaterial.h"

#include <ctime>

#include <glad/glad.h>

namespace Misaka {

ShaderToyMaterial::ShaderToyMaterial(std::shared_ptr<Shader> shader)
    : MaterialBase(shader) {}

void ShaderToyMaterial::uploadUniforms(const MaterialContext& context) {
    UploadCommonContextUniforms(context);

    for (int i = 0; i < 4; ++i) {
        if (channels[i] != 0) {
            glActiveTexture(GL_TEXTURE4 + i);
            glBindTexture(GL_TEXTURE_2D, channels[i]);
        }
        const std::string channelName = "iChannel" + std::to_string(i);
        _shader->SetInt(channelName.c_str(), 4 + i);

        std::string channelTime = "iChannelTime[" + std::to_string(i) + "]";
        std::string channelRes = "iChannelResolution[" + std::to_string(i) + "]";
        _shader->SetFloat(channelTime.c_str(), context.time);
        _shader->SetVec3(channelRes.c_str(), glm::vec3(0.0f));
    }

    float frameRate = context.deltaTime > 0.000001f ? (1.0f / context.deltaTime) : 0.0f;
    _shader->SetFloat("iFrameRate", frameRate);

    std::time_t now = std::time(nullptr);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    float secondsInDay = static_cast<float>(localTime.tm_hour * 3600 + localTime.tm_min * 60 + localTime.tm_sec);
    _shader->SetVec4("iDate", glm::vec4(
        static_cast<float>(localTime.tm_year + 1900),
        static_cast<float>(localTime.tm_mon + 1),
        static_cast<float>(localTime.tm_mday),
        secondsInDay
    ));

    UploadCustomUniforms();
}

} // namespace Misaka
