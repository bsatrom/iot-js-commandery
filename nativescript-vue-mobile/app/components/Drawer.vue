<template>
  <Page>
    <ActionBar>
      <GridLayout width="100%" columns="auto, *">
        <Label text="..." @tap="$refs.drawer.nativeView.showDrawer()" col="0" />
        <Label class="title" text="Particle Dock Controller" col="1" />
      </GridLayout>
    </ActionBar>

    <RadSideDrawer ref="drawer">
      <StackLayout ~drawerContent backgroundColor="#ffffff">
        <Label class="drawer-header" text="Dock Options" />

        <Button class="drawer-item" text="Env Vals" @tap="$navigateTo(envValsPage)" />
        <Button class="drawer-item" text="Light Manager" @tap="$navigateTo(lightManagerPage)" />
        <Button class="drawer-item" text="OLED Controller" @tap="$navigateTo(oledPage)" />
      </StackLayout>

      <slot>
        <p>Empty!</p>
      </slot>
    </RadSideDrawer>
  </Page>
</template>

<style scoped>
ActionBar {
  background-color: #53ba82;
  color: #ffffff;
}

.title {
  text-align: left;
  padding-left: 16;
}

.message {
  vertical-align: center;
  text-align: center;
  font-size: 20;
  color: #333333;
}

.drawer-header {
  padding: 50 16 16 16;
  margin-bottom: 16;
  background-color: #53ba82;
  color: #ffffff;
  font-size: 24;
}

.drawer-item {
  padding: 8 16;
  color: #333333;
  font-size: 16;
}
</style>
