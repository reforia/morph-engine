#include "Texture.hpp"

template<typename TexType, typename ...Args>
typename TexType::sptr_t createOrFail(const void* data, size_t size, Args ... args) {
  typename TexType::sptr_t tex = TexType::sptr_t(new TexType(args..., data, size));
  return tex->rhiInit(data, size) ? tex : nullptr;
}


Texture2::sptr_t Texture2::create(uint width, uint height, eTextureFormat format, BindingFlag flag, const void* data, size_t size) {
  return createOrFail<Texture2>(data, size, width, height, format, flag);
}

Texture2::sptr_t Texture2::create(rhi_resource_handle_t res) {
  return Texture2::sptr_t(new Texture2(res));
}

const RenderTargetView& Texture2::rtv() {
  if (!mRtv) {
    mRtv = RenderTargetView::create(shared_from_this());
  }
  return *mRtv;
}

const DepthStencilView* Texture2::dsv() {
  if (mFormat != TEXTURE_FORMAT_D24S8) return nullptr;

  if(!mDsv) {
    mDsv = DepthStencilView::create(shared_from_this());
  }

  return mDsv.get();
}