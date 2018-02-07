#include "UIRenderer.h"
#include "VulkanRenderer/GpuBuffer.h"
#include "Resource/RenderPass/RenderPassUI.h"
#include "Graphics/Render/RenderManager.h"
#include "Core/Game.h"

namespace Origin {

UIRenderer::UIRenderer() {
    renderPassUI = std::make_unique<RenderPassUI>(Game::getRenderManager()->getGraphicsDevice());
}

UIRenderer::~UIRenderer() {

}

void UIRenderer::addBatch(UIBatch batch) {
    batches.push_back(batch);
}

void UIRenderer::drawBatches() {
    uint32_t size = vertices.size() * sizeof(UIBatch::Vertex);

    if (size > renderPassUI->getVertexBuffer()->getSize()) {
        renderPassUI->resizeVertexBuffer(size);
    }

    if (size) {
        renderPassUI->getVertexBuffer()->write(vertices.data(), size);
        renderPassUI->setVertexCount(vertices.size());
        renderPassUI->setTexture(batches.at(0).texture); // TODO: Sort batches and swith textures
    }

    batches.clear();
    vertices.clear();
}

} // Origin
