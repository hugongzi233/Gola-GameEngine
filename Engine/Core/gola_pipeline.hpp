#pragma once
#include "gola_device.hpp"
#include <string>
#include <vector>

namespace gola {
	struct PipelineConfigInfo {
		VkViewport viewport;
		VkRect2D scissor;
		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
		VkPipelineRasterizationStateCreateInfo rasterizationInfo;
		VkPipelineMultisampleStateCreateInfo multisampleInfo;
		VkPipelineColorBlendAttachmentState colorBlendAttachment;
		VkPipelineColorBlendStateCreateInfo colorBlendInfo;
		VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
		std::vector<VkDynamicState> dynamicStateEnables;
		VkPipelineDynamicStateCreateInfo dynamicStateInfo;
		VkPipelineLayout pipelineLayout = nullptr;
		VkRenderPass renderPass = nullptr;
		uint32_t subpass = 0;
	};

	class GolaPipeline {
	public:
		GolaPipeline(
			GolaDevice& device,
			const std::string& vertexShaderPath,
			const std::string& fragmentShaderPath,
			const PipelineConfigInfo& configInfo);

		~GolaPipeline();

		GolaPipeline(const GolaPipeline&) = delete;
		GolaPipeline& operator=(const GolaPipeline&) = delete;

		void bind(VkCommandBuffer commandBuffer);

		static PipelineConfigInfo defaultPipelineConfigInfo(uint32_t width, uint32_t height);

	private:
		static std::vector<char> readFile(const std::string& filepath);

		void createGraphicsPipeline(const std::string& vertexShaderPath,
			const std::string& fragmentShaderPath,
			const PipelineConfigInfo& configInfo);

		void createShaderModule(const std::vector<char>& code, VkShaderModule& shaderModule);

		GolaDevice& golaDevice;
		VkPipeline graphicsPipeline;
		VkShaderModule vertexShaderModule;
		VkShaderModule fragmentShaderModule;
	};
}