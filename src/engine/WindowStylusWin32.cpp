#include <GLFW/glfw3.h>
#include <autograph/engine/Window.h>
#include <autograph/support/Debug.h>
#include <autograph/support/ProjectRoot.h>
#include <codecvt>
#include <locale>

#ifdef WIN32
#include <ole2.h>
#include <rtscom.h>
#include <rtscom_i.c>
#endif

namespace ag {

#ifdef WIN32
/*class CRTSEventHandler : public IStylusSyncPlugin {
public:
  CRTSEventHandler() : m_nRef(1), m_pPunkFTMarshaller(NULL) {}
  virtual ~CRTSEventHandler() {
    if (m_pPunkFTMarshaller != NULL)
      m_pPunkFTMarshaller->Release();
  }

  // IStylusSyncPlugin inherited methods

  // Methods whose data we use
  STDMETHOD(Packets)
  (IRealTimeStylus *pStylus, const ::StylusInfo *pStylusInfo, ULONG nPackets,
   ULONG nPacketBuf, LONG *pPackets, ULONG *nOutPackets, LONG **ppOutPackets) {
    fmt::print("Packets!\n");
    return S_OK;
  }

  STDMETHOD(InAirPackets)
  (IRealTimeStylus *pStylus, const ::StylusInfo *pStylusInfo, ULONG nPackets,
   ULONG nPacketBuf, LONG *pPackets, ULONG *nOutPackets, LONG **ppOutPackets) {
    fmt::print("In air packet!\n");
    return S_OK;
  }
  STDMETHOD(DataInterest)(RealTimeStylusDataInterest *pEventInterest) {
    *pEventInterest =
        (RealTimeStylusDataInterest)(RTSDI_Packets | RTSDI_InAirPackets);
    return S_OK;
  }

  // Methods you can add if you need the alerts - don't forget to change
  // DataInterest!
  STDMETHOD(StylusDown)
  (IRealTimeStylus *, const StylusInfo *, ULONG, LONG *_pPackets, LONG **) {
    return S_OK;
  }
  STDMETHOD(StylusUp)
  (IRealTimeStylus *, const StylusInfo *, ULONG, LONG *_pPackets, LONG **) {
    return S_OK;
  }
  STDMETHOD(RealTimeStylusEnabled)
  (IRealTimeStylus *, ULONG, const TABLET_CONTEXT_ID *) { return S_OK; }
  STDMETHOD(RealTimeStylusDisabled)
  (IRealTimeStylus *, ULONG, const TABLET_CONTEXT_ID *) { return S_OK; }
  STDMETHOD(StylusInRange)(IRealTimeStylus *, TABLET_CONTEXT_ID, STYLUS_ID) {
    return S_OK;
  }
  STDMETHOD(StylusOutOfRange)(IRealTimeStylus *, TABLET_CONTEXT_ID, STYLUS_ID) {
    return S_OK;
  }
  STDMETHOD(StylusButtonUp)
  (IRealTimeStylus *, STYLUS_ID, const GUID *, POINT *) {
    return S_OK;
  }
  STDMETHOD(StylusButtonDown)
  (IRealTimeStylus *, STYLUS_ID, const GUID *, POINT *) { return S_OK; }
  STDMETHOD(SystemEvent)
  (IRealTimeStylus *, TABLET_CONTEXT_ID, STYLUS_ID, SYSTEM_EVENT,
   SYSTEM_EVENT_DATA) {
    return S_OK;
  }
  STDMETHOD(TabletAdded)(IRealTimeStylus *, IInkTablet *) { return S_OK; }
  STDMETHOD(TabletRemoved)(IRealTimeStylus *, LONG) { return S_OK; }
  STDMETHOD(CustomStylusDataAdded)
  (IRealTimeStylus *, const GUID *, ULONG, const BYTE *) { return S_OK; }
  STDMETHOD(Error)
  (IRealTimeStylus *, IStylusPlugin *, RealTimeStylusDataInterest, HRESULT,
   LONG_PTR *) {
    return S_OK;
  }
  STDMETHOD(UpdateMapping)(IRealTimeStylus *) { return S_OK; }

  // IUnknown methods
  STDMETHOD_(ULONG, AddRef)() { return InterlockedIncrement(&m_nRef); }
  STDMETHOD_(ULONG, Release)() {
    ULONG nNewRef = InterlockedDecrement(&m_nRef);
    if (nNewRef == 0)
      delete this;

    return nNewRef;
  }
  STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObj) {
    if ((riid == IID_IStylusSyncPlugin) || (riid == IID_IUnknown)) {
      *ppvObj = this;
      AddRef();
      return S_OK;
    } else if ((riid == IID_IMarshal) && (m_pPunkFTMarshaller != NULL)) {
      return m_pPunkFTMarshaller->QueryInterface(riid, ppvObj);
    }

    *ppvObj = NULL;
    return E_NOINTERFACE;
  }

  LONG m_nRef;                   // COM object reference count
  IUnknown *m_pPunkFTMarshaller; // free-threaded marshaller
};

struct Window::StylusState {
  HWND hwnd;
  IRealTimeStylus *rtStylus;
  std::unique_ptr<CRTSEventHandler> rtStylusEvHandler;

  void init(HWND hwnd_) {
    hwnd = hwnd;

    HRESULT hr = CoCreateInstance(CLSID_RealTimeStylus, NULL, CLSCTX_ALL,
                                  IID_PPV_ARGS(&rtStylus));
    if (FAILED(hr))
      AG_DEBUG("CoCreateInstance failed (HRESULT {})", hr);

    hr = rtStylus->put_HWND((HANDLE_PTR)hwnd);
    if (FAILED(hr))
      AG_DEBUG("put_HWND failed (HRESULT {})", hr);

    // Create eventhandler
    rtStylusEvHandler = std::make_unique<CRTSEventHandler>();

    // Create free-threaded marshaller for this object and aggregate it.
    // (whatever that means)
    hr = CoCreateFreeThreadedMarshaler(rtStylusEvHandler.get(),
                                       &rtStylusEvHandler->m_pPunkFTMarshaller);
    if (FAILED(hr))
      AG_DEBUG("CoCreateFreeThreadedMarshaler failed (HRESULT {})", hr);

    // Add handler object to the list of synchronous plugins in the RTS object.
    hr = rtStylus->AddStylusSyncPlugin(0, rtStylusEvHandler.get());
    if (FAILED(hr))
      AG_DEBUG("AddStylusSyncPlugin failed (HRESULT {})", hr);

    // Set data we want - we're not actually using all of this, but we're gonna
    // get X and Y anyway so might as well set it
    GUID wanted_props[] = {GUID_PACKETPROPERTY_GUID_X,
                           GUID_PACKETPROPERTY_GUID_Y,
                           GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE,
                           GUID_PACKETPROPERTY_GUID_X_TILT_ORIENTATION};
    rtStylus->SetDesiredPacketDescription(4, wanted_props);
    rtStylus->put_Enabled(true);

    std::vector<TABLET_CONTEXT_ID> tabletContexts;

    {
      ULONG nTabletContexts = 0;
      TABLET_CONTEXT_ID *piTabletContexts;
      HRESULT res =
          rtStylus->GetAllTabletContextIds(&nTabletContexts, &piTabletContexts);
      for (ULONG i = 0; i < nTabletContexts; i++) {
        IInkTablet *pInkTablet;
        if (SUCCEEDED(rtStylus->GetTabletFromTabletContextId(
                piTabletContexts[i], &pInkTablet))) {
          float ScaleX, ScaleY;
          ULONG nPacketProps;
          PACKET_PROPERTY *pPacketProps;
          BSTR tabletName;
          pInkTablet->get_Name(&tabletName);
          auto tabletNameStr =
              std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(
                  (const wchar_t *)tabletName);
          AG_DEBUG("Tablet Name: {}", tabletNameStr);
          rtStylus->GetPacketDescriptionData(piTabletContexts[i], &ScaleX,
                                             &ScaleY, &nPacketProps,
                                             &pPacketProps);
          for (ULONG j = 0; j < nPacketProps; j++) {
            AG_DEBUG("Property #{}, metrics {} {} {} {}", j,
                     pPacketProps[j].PropertyMetrics.fResolution,
                     pPacketProps[j].PropertyMetrics.nLogicalMin,
                     pPacketProps[j].PropertyMetrics.nLogicalMax,
                     pPacketProps[j].PropertyMetrics.Units);
            if (pPacketProps[j].guid !=
                GUID_PACKETPROPERTY_GUID_NORMAL_PRESSURE)
              continue;
          }
          CoTaskMemFree(pPacketProps);
        }
      }
    }
  }
};*/

#else

void Window::initStylus() {}

StylusInfo Window::getStylusInfo(int stylus) {
  StylusInfo si;
  glfwGetCursorPos(window_, &si.x, &si.y);
  si.pressure = 0.0;
  return si;
}

#endif
}