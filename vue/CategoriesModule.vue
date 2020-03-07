<template>
    <div>
        <loading-overlay v-if="!items || !roles"></loading-overlay>
        <div v-else class="row">
            <div class="col-lg-4 col-sm-6" v-for="category in items" :key="category.id">
                <category-card
                    v-bind.sync="category"
                    :roles="roles"
                    @input="update(category)"
                    @remove="remove(category)"
                >
                </category-card>
            </div>
            <div class="col-lg-4 col-sm-6 create-category-card-container">
                <create-category-card @click.native="create"></create-category-card>
            </div>
            <save-popup :visible="isSavePopupVisible" @save="save" @cancel="cancel" :loading="isSaving"></save-popup>
        </div>
    </div>
</template>
<script>
import { BaseCheckbox, BaseRadio } from '../../components/index';
import CategoryCard from "../../components/Cards/CategoryCard";
import CreateCategoryCard from "../../components/Cards/CreateCategoryCard";
import SavePopup from "../../components/Popups/SavePopup";
import LoadingOverlay from "../../components/LoadingOverlay";

export default {
  components: {
    LoadingOverlay,
    SavePopup,
    CreateCategoryCard,
    CategoryCard,
    BaseCheckbox,
    BaseRadio
  },
  computed: {
    isSavePopupVisible: function () {
      return !_.isEqual(this.items, this.itemsBackup);
    }
  },
  data() {
    return {
      roles: null,
      items: null,
      itemsBackup: null,
      isSaving: false,
      itemsToCreate: new Set(),
      itemsToUpdate: new Set(),
      itemsToRemove: new Set(),
    };
  },
  methods: {
    refresh() {
      this.$api.get('servers/' + this.$route.params.id + '/roles')
        .then(response => {
          this.roles = response.data.data;
          this.roles.forEach((role) => {
            role.label = role.display_name || role.name;
          });
          this.$api.get('servers/' + this.$route.params.id + '/categories')
            .then(response => {
              this.items = response.data.data;
              this.items.forEach(item => {
                item.properties = item.properties.split(',');

                let roleIds = item.default_for_roles.split(',').map((roleId) => parseInt(roleId));
                item.default_for_roles = this.roles.filter((role) => roleIds.some((roleId) => roleId === role.id));
              });
              this.items.sort((a, b) => { return a.id > b.id ? 1 : -1 });
              this.itemsBackup = JSON.parse(JSON.stringify(this.items));
            })
        })
    },
    create() {
      let newItem = {name: 'Новая категория', icon: '', posts_count: null, properties: [], default_for_roles: []};
      this.items.push(newItem);
      this.itemsToCreate.add(newItem);
    },
    update(item) {
      if (this.itemsToCreate.has(item)) return;
      if (!_.isEqual(item, this.itemsBackup.find(itemBackup => itemBackup.id === item.id))) {
        this.itemsToUpdate.add(item);
      } else {
        this.itemsToUpdate.delete(item);
      }
    },
    remove(item) {
      this.itemsToUpdate.delete(item);
      if (!this.itemsToCreate.has(item)) this.itemsToRemove.add(item);
      this.itemsToCreate.delete(item);

      let index = this.items.indexOf(item);
      this.items.splice(index, 1);
    },
    save() {
      this.isSaving = true;
      let promises = [];

      this.itemsToUpdate.forEach(item => { promises.push(this.$api.put('servers/' + this.$route.params.id + '/categories/' + item.id, {name: item.name, icon: item.icon, posts_count: item.posts_count, properties: item.properties.join(','), default_for_roles: item.default_for_roles.map((role) => role.id).join(',') })) });
      this.itemsToUpdate.clear();

      this.itemsToRemove.forEach(item => { promises.push(this.$api.delete('servers/' + this.$route.params.id + '/categories/' + item.id)) });
      this.itemsToRemove.clear();


      this.itemsToCreate.forEach(item => { promises.push(this.$api.post('servers/' + this.$route.params.id + '/categories/', {name: item.name, icon: item.icon, posts_count: item.posts_count, properties: item.properties.join(','), default_for_roles: item.default_for_roles.map((role) => role.id).join(',')  })) });
      this.itemsToCreate.clear();

      Promise.all(promises).then(() => {
        this.isSaving = false;
        this.itemsBackup = JSON.parse(JSON.stringify(this.items));

        this.refresh();
      });
    },
    cancel() {
      this.items = JSON.parse(JSON.stringify(this.itemsBackup));
      this.itemsToUpdate.clear();
      this.itemsToCreate.clear();
      this.itemsToRemove.clear();

      this.isSaving = false;
    }
  },
  mounted() {
    this.refresh();
  }
};
</script>
<style scoped>

</style>
