include $(TOPDIR)/rules.mk
 
PKG_NAME:=Probe-App
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)
 
include $(INCLUDE_DIR)/package.mk
 
define Package/Probe-App
    SECTION:=utils
    CATEGORY:=Utilities
    TITLE:=$(PKG_NAME)
    DEPENDS:=
endef
 
define Build/Prepare
	$(MKDIR) -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef
 
define Package/Probe-App/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin
endef


$(eval $(call BuildPackage,Probe-App))
