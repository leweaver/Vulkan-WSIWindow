// * Copyright (C) 2019 by Rene Lindsay
//
//      -- Renderpasss structure --
//
//                    +-------------------------+
//                    | VkAttachmentDescription |
//                    +-----+--------------+----+
//                          | 1            V m
//                          ^ m            |
//           +--------------+--------+     |
//           | VkAttachmentReference |     |
//           +--------------+--------+     |
//                          V m            |
//                          | 1            |
//           +--------------+-------+      |
//           | VkSubpassDescription |      |
//           +---------+----+-------+      |
//                     V 2  V m            |
//                     | 1  |              |
//  +------------------+--+ |              |
//  | VkSubpassDependency | |              |
//  +------------------+--+ |              |
//                     V m  |              |
//                     |    |              |
//                     | 1  | 1            | 1
//                   +------+--------------+--+
//                   | VkRenderPassCreateInfo |
//                   +------------------------+
//
// How to use:
//   1: Find which device can present to given surface, and what color/depth formats it supports.
//   2: Create an instance of renderpass. eg: CRenderpass renderpass(device);
//   3: Add Color and Depth-stencil attachments.
//   4: Add subpasses, with an array of attachment indexes, used by that renderpass.
//
// eg:
//    CRenderpass renderpass(device);                              // Create a new renderpass structure.
//    renderpass.AddColorAttachment(VK_FORMAT_B8G8R8A8_UNORM);     // Add a color attachment
//    renderpass.AddDepthAttachment(VK_FORMAT_D24_UNORM_S8_UINT);  // Add a depth-stencil attachment
//    renderpass.AddSubpass({0,1});                                // Create subpass, and link to attachment 0 and 1. (color and depth)
//    renderpass.Create();                                         // Create the VkRenderPass instance. (optional)
//
//    NOTE: Get supported formats from CPhysicalDevice, rather than hard-coding them.
//    WARNING: Don't make changes to renderpass after passing it to CSwapchain or CPipeline
//
// TODO:
//   Subpass.pResolveAttachments
//   Subpass.pPreserveAttachments


#ifndef CRENDERPASS_H
#define CRENDERPASS_H

#include "WSIWindow.h"

class CRenderpass {
    class CSubpass {
        friend class CRenderpass;
        CRenderpass& renderpass;
        std::vector<VkAttachmentReference> input_refs;
        std::vector<VkAttachmentReference> color_refs;
        VkAttachmentReference              depth_ref;
        VkAttachmentReference*             pdepth_ref;  // points to depth_ref, or 0 if none.
        operator VkSubpassDescription();
        CSubpass(CRenderpass& renderpass);
      public:
        void UseAttachment(uint32_t attachment_index);  // for write
        void UseAttachments(vector<uint32_t> attachment_indexes = {});
        void InputAttachment(uint32_t attachment_index);  //for read
        void InputAttachments(vector<uint32_t> attachment_indexes = {});
    };

    VkDevice     device;
    VkRenderPass renderpass;

  public:
    // ---Used by CSwapchain ---
    VkFormat surface_format = VK_FORMAT_UNDEFINED;
    VkFormat depth_format   = VK_FORMAT_UNDEFINED;
    std::vector<VkClearValue> clearValues;
    //--------------------------

    std::vector<CSubpass>                subpasses;
    std::vector<VkAttachmentDescription> attachments;
    std::vector<VkSubpassDependency>     dependencies;

    CRenderpass(VkDevice device);
    ~CRenderpass();

    uint32_t AddColorAttachment(VkFormat format, VkClearColorValue clearVal = {}, VkImageLayout final_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    uint32_t AddDepthAttachment(VkFormat format, VkClearDepthStencilValue clearVal = {1.f, 0});
    CSubpass& AddSubpass(vector<uint32_t> attachment_indexes = {});
    void AddSubpassDependency(uint32_t srcSubpass, uint32_t dstSubpass);

    void Create();
    void Destroy();
    operator VkRenderPass () {
        if(!renderpass) Create();
        return renderpass;
    }
};

#endif
