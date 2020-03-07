<template>
    <div>
        <loading-overlay v-if="loading"></loading-overlay>
        <div v-if="!loading" class="row">
            <div class="col-lg-4 col-sm-6" v-for="(list, index) in lists" :key="list.id">
                <small-card
                    :id="index + 1"
                    :name="list.name"
                    module="lists"
                    :editable="!!list.id"
                    @remove="itemToRemove = list; confirmationPopupVisible = true"
                ></small-card>
            </div>
            <div class="col-lg-4 col-sm-6 create-category-card-container">
                <create-card text="Создать список"  @click.native="creationPopupVisible = true; itemToCreate = {name: null, list_id: null}"></create-card>
            </div>
        </div>
        <div v-if="!loading" class="row">
            <div class="col-lg-8">
                <card title="Настройки">
                    <span class="group-name">Общие настройки</span>
                    <input-field v-model="settings.list_cache_lifetime.value" caption="Актуальность старых списков" description="Этот параметр определяет, как долго будут храниться и засчитываться старые списки." placeholder="в минутах"></input-field>
                </card>
            </div>
        </div>
        <window-popup title="Создание списка" :visible="creationPopupVisible" id="create-list-popup" @cancel="creationPopupVisible = false">
            <form @submit.prevent>
                <base-input label="Имя списка" placeholder="Имя списка" v-model="itemToCreate.name"></base-input>
                <base-button class="mt-3 float-right" native-type="submit" type="primary" @click="create(); creationPopupVisible = false" :disabled="!itemToCreate.name">Создать</base-button>
            </form>
        </window-popup>
        <window-popup title="Подтверждение" :visible="confirmationPopupVisible" id="confirmation-popup" @cancel="confirmationPopupVisible = false">
            <div class="d-flex align-items-center">
                <i class="danger-icon tim-icons icon-alert-circle-exc"></i>
                <p class="m-0 pl-3">Вы действительно хотите удалить список "{{ itemToRemove.name }}"?</p>
            </div>
            <div class="d-flex mt-3">
                <base-button class="ml-auto" @click="confirmationPopupVisible = false">Отмена</base-button>
                <base-button class="ml-3" type="danger" @click="remove(); confirmationPopupVisible = false">Удалить</base-button>
            </div>
        </window-popup>
        <save-popup :visible="isSavePopupVisible" @save="save" @cancel="cancel" :loading="isSaving"></save-popup>
    </div>
</template>
<script>
import { BaseCheckbox, BaseRadio } from '../../components/index';
import ListCard from "../../components/Cards/ListCard";
import CreateCard from "../../components/Cards/CreateCard";
import SmallCard from "../../components/Cards/SmallCard";
import WindowPopup from "../../components/Popups/WindowPopup";
import SavePopup from "../../components/Popups/SavePopup";
import LoadingOverlay from "../../components/LoadingOverlay";
import InputField from "../../components/InputField";

export default {
  components: {
    InputField,
    LoadingOverlay,
    SavePopup,
    WindowPopup,
    SmallCard,
    ListCard,
    CreateCard,
    BaseCheckbox,
    BaseRadio
  },
  computed: {
    isSavePopupVisible: function() {
      return !_.isEqual(this.lists, this.listsBackup) || !_.isEqual(this.settings, this.settingsBackup);
    },
    loading: function () {
      return !this.lists || !this.settings;
    }
  },
  data() {
    return {
      isSaving: false,
      creationPopupVisible: false,
      confirmationPopupVisible: false,
      itemToCreate: {},
      itemToRemove: {},
      itemsToCreate: new Set(),
      itemsToRemove: new Set(),
      lists: null,
      listsBackup: [],
      settings: null,
      settingsBackup: null
    };
  },
  methods: {
    refresh() {
      this.$api.get('servers/' + this.$route.params.id + '/lists')
        .then(response => {
          this.lists = response.data.data;
          this.listsBackup = JSON.parse(JSON.stringify(this.lists));

            this.$api.get('servers/' + this.$route.params.id + '/settings', { data: { object_type: 'module', object_id: 1 }})
              .then(response => {
                let settings = {};
                response.data.data.forEach(s => {
                  settings[s.name] = s;
                });
                this.settings = settings;
                this.settingsBackup = JSON.parse(JSON.stringify(this.settings));
                this.isSaving = false;
              })
        });
    },
    create() {
      this.itemsToCreate.add(this.itemToCreate);
      this.lists.push(this.itemToCreate);

      this.itemToCreate = {};
    },
    remove() {
      if (!this.itemsToCreate.has(this.itemToRemove)) {
        this.itemsToRemove.add(this.itemToRemove);
      }
      this.itemsToCreate.delete(this.itemToRemove);

      this.lists.splice(this.lists.indexOf(this.itemToRemove), 1);
    },
    save() {
      this.isSaving = true;
      let promises = [];

      this.itemsToCreate.forEach((item) => {
        promises.push(this.$api.post('servers/' + this.$route.params.id + '/lists', { name: item.name }));
      });

      this.itemsToRemove.forEach((item) => {
        promises.push(this.$api.delete('servers/' + this.$route.params.id + '/lists/' + item.id))
      });

      promises.push(this.$api.put('servers/' + this.$route.params.id + '/settings', { settings: Object.values(this.settings) }));

      Promise.all(promises).then(response => {
        this.refresh();
        this.itemsToRemove.clear();
        this.itemsToCreate.clear();

      });
    },
    cancel() {
      this.lists = JSON.parse(JSON.stringify(this.listsBackup));
      this.settings = JSON.parse(JSON.stringify(this.settingsBackup));
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
