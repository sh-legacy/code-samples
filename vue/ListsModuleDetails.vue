<template>
    <div>
        <loading-overlay v-if="!list || !categories"></loading-overlay>
        <div v-if="list && categories" class="row">
            <div class="col-lg-7">
                <div class="row">
                    <div class="col-12">
                        <card header-classes="d-flex p-0 align-items-center" body-classes="pl-sm-lg">
                            <template v-slot:header>
                                <h4 class="card-id m-0">7</h4>
                                <base-input v-model="list.name" class="flex-grow-1 mb-0 mx-3"> </base-input>
                            </template>
                            <label>Версия</label>
                            <template v-if="versionsUiVisible">
                                <div>
                                    <el-select
                                        class="select-primary"
                                        size="large"
                                        placeholder="Версия списка"
                                        v-model="selectedVersionForEditing"
                                        value-key="id"
                                        default-first-option
                                    >
                                        <el-option
                                            v-for="version in list.versions"
                                            class="select-primary"
                                            :value="version"
                                            :label="version.display_name"
                                            :key="version.id"
                                        >
                                        </el-option>
                                    </el-select>
                                    <label>Короткое имя версии</label>
                                    <base-input v-model="selectedVersionForEditing.name" :disabled="selectedVersionForEditing.name === 'default'"> </base-input>
                                    <label>Имя для отображения в интерфейсе</label>
                                    <base-input v-model="selectedVersionForEditing.display_name"> </base-input>

                                    <a href="#" @click="createVersion" class="secondary-link">Создать версию</a>
                                    <span v-if="selectedVersionForEditing.name !== 'default'">⋅</span>
                                    <a v-if="selectedVersionForEditing.name !== 'default'" href="#" @click="removeVersionPopupVisible = true" class="secondary-link">Удалить версию</a>
                                    <span v-if="hideVersionsUiButtonVisible">⋅</span>
                                    <a href="#" v-if="hideVersionsUiButtonVisible" @click="showVersionsUi = false" class="secondary-link">Скрыть интерфейс версий</a>
                                </div>
                            </template>
                            <div v-else class="hint">
                                <i class="tim-icons icon-bulb-63"></i>
                                <div>
                                    <p>Вы можете создать несколько версий одного списка с разными макетами, но общим набором категорий и пользователей.</p>
                                    <a href="#" @click="showVersionsUi = true">Показать интерфейс версий</a>
                                </div>
                            </div>
                            <label for="layout-textarea">Макет</label>
                            <div class="textarea-wrapper" id="layout-textarea-wrapper">
                                <textarea id="layout-textarea" v-model="selectedVersionForEditing.layout" @input="adjustTextarea('layout-textarea')" placeholder="Макет списка"></textarea>
                            </div>
                        </card>
                    </div>
                    <div class="col-12">
                        <card title="Текст списка" sub-title="Здесь отображается актуальный текст списка, сгенерированный на основе вашего макета и пользователей, которые входят в список.">
                            <template v-if="list.versions.length > 1">
                                <label>Версия</label>
                                <el-select
                                    class="select-primary"
                                    size="large"
                                    placeholder="Версия списка"
                                    v-model="selectedVersionForText"
                                    value-key="id"
                                    default-first-option
                                >
                                    <el-option
                                        v-for="version in list.versions"
                                        class="select-primary"
                                        :value="version"
                                        :label="version.display_name"
                                        :key="version.id"
                                    >
                                    </el-option>
                                </el-select>
                            </template>
                            <label>Текст</label>
                            <div class="textarea-wrapper" id="listtext-textarea-wrapper" :class="{'textarea-disabled': !isCurrentListTextAvailable}">
                                <textarea id="listtext-textarea" :value="currentListText" readonly></textarea>
                            </div>
                            <el-tooltip
                                content="Скопировано!"
                                effect="light"
                                :manual="true"
                                v-model="tooltipVisible"
                                placement="bottom"
                            >
                                <base-button class="float-right mt-3" native-type="submit" type="primary" @click="copy" :disabled="!isCurrentListTextAvailable">
                                    <i class="tim-icons icon-single-copy-04 pr-1"></i>Скопировать
                                </base-button>
                            </el-tooltip>
                        </card>
                    </div>
                </div>
            </div>
            <div class="col-lg-5 create-category-card-container" v-if="usersCardVisible">
                <card title="Пользователи">
                    <p v-if="list.categories.length === 0" class="text-center mb-4">Здесь пока пусто.</p>
                    <draggable
                        :list="list.categories"
                        draggable=".category-draggable"
                        ghostClass="draggable-source--is-dragging"
                    >
                        <template v-for="category in list.categories">
                            <div class="category-draggable">
                                <p class="category-delimiter users-list">{{ category.display_name || category.name }}</p>
                                <draggable
                                    :list="category.users"
                                    :key="category.id"
                                    group="users-list"
                                    tag="ul"
                                    ghostClass="draggable-source--is-dragging"
                                >
                                    <li
                                        v-for="user in category.users"
                                        :key="user.id"
                                        class="user-list-item align-items-center users-list"
                                    >
                                        <i class="far fa-user mr-2 role-icon"></i>
                                        <p class="user-name">{{ user.first_name }} {{ user.last_name }}</p>
                                        <i class="tim-icons icon-simple-remove remove-icon ml-auto" @click="userToRemove = user; removeUserPopupVisible = true;"></i>
                                    </li>
                                </draggable>
                            </div>
                        </template>
                        <div  v-if="usersWithoutCategory.length > 0">
                            <p class="category-delimiter users-list">Без категорий</p>
                            <draggable
                                :list="usersWithoutCategory"
                                group="users-list"
                                tag="ul"
                                ghostClass="draggable-source--is-dragging"
                            >
                                <li
                                    v-for="user in usersWithoutCategory"
                                    :key="user.id"
                                    class="user-list-item align-items-center users-list"
                                >
                                    <i class="far fa-user mr-2 role-icon"></i>
                                    <p class="user-name">{{ user.first_name }} {{ user.last_name }}</p>
                                    <i class="tim-icons icon-simple-remove remove-icon ml-auto" @click="userToRemove = user; removeUserPopupVisible = true;"></i>
                                </li>
                            </draggable>
                        </div>
                    </draggable>
                    <a v-if="list.categories.length < categories.length" href="#" @click="addCategoryPopupVisible = true" class="secondary-link">Добавить категорию</a>
                    <span v-if="list.categories.length < categories.length && list.categories.length > 0">⋅</span>
                    <a v-if="list.categories.length > 0" href="#" @click="removeCategoryPopupVisible = true" class="secondary-link">Удалить категорию</a>
                </card>
            </div>
            <window-popup title="Подтверждение" :visible="removeVersionPopupVisible" id="remove-version-popup" @cancel="removeVersionPopupVisible = false">
                <div class="d-flex align-items-center">
                    <i class="danger-icon tim-icons icon-alert-circle-exc"></i>
                    <p class="m-0 pl-3">Вы действительно хотите удалить версию списка "{{ selectedVersionForEditing.name }}"?</p>
                </div>
                <div class="d-flex mt-3">
                    <base-button class="ml-auto" @click="removeVersionPopupVisible = false">Отмена</base-button>
                    <base-button class="ml-3" type="danger" @click="removeVersionPopupVisible = false; removeVersion()">Удалить</base-button>
                </div>
            </window-popup>
            <window-popup title="Подтверждение" :visible="removeUserPopupVisible" id="remove-user-popup" @cancel="removeUserPopupVisible = false">
                <div class="d-flex align-items-center">
                    <i class="danger-icon tim-icons icon-alert-circle-exc"></i>
                    <p class="m-0 pl-3">Вы действительно хотите исключить пользователя {{ userToRemove.first_name + ' ' + userToRemove.last_name }}?</p>
                </div>
                <div class="d-flex mt-3">
                    <base-button class="ml-auto" @click="removeUserPopupVisible = false">Отмена</base-button>
                    <base-button class="ml-3" type="danger" @click="removeUserPopupVisible = false; removeUser()">Да</base-button>
                </div>
            </window-popup>
            <window-popup title="Добавить категорию" :visible="addCategoryPopupVisible" id="add-category-popup" @cancel="addCategoryPopupVisible = false; categoryToAdd = null">
                <form @submit.prevent>
                    <div>
                        <label class="d-block mt-n3">Категория</label>
                        <el-select
                            class="select-primary switch chat-purpose-select"
                            size="large"
                            placeholder="Нет"
                            v-model="categoryToAdd"
                            value-key="id"
                        >
                            <el-option
                                v-for="category in categoriesAvailableToAdd"
                                class="select-primary"
                                :value="category"
                                :label="category.display_name || category.name"
                                :key="category.id"
                            >
                            </el-option>
                        </el-select>
                        <base-button class="mt-3 float-right" type="primary" @click="addCategory()" :disabled="!categoryToAdd">Добавить</base-button>
                    </div>
                </form>
            </window-popup>
            <window-popup title="Удалить категорию" :visible="removeCategoryPopupVisible" id="remove-category-popup" @cancel="removeCategoryPopupVisible = false; categoryToRemove = null">
                <form @submit.prevent>
                    <div>
                        <label class="d-block mt-n3">Категория</label>
                        <el-select
                            class="select-primary switch chat-purpose-select"
                            size="large"
                            placeholder="Нет"
                            v-model="categoryToRemove"
                            value-key="id"
                        >
                            <el-option
                                v-for="category in list.categories"
                                class="select-primary"
                                :value="category"
                                :label="category.display_name || category.name"
                                :key="category.id"
                            >
                            </el-option>
                        </el-select>
                        <base-button class="mt-3 float-right" type="primary" @click="removeCategory()" :disabled="!categoryToRemove">Удалить</base-button>
                    </div>
                </form>
            </window-popup>
        </div>
        <save-popup :visible="isSavePopupVisible" @save="save" @cancel="cancel" :loading="isSaving"></save-popup>
    </div>
</template>
<script>
import { BaseCheckbox, BaseRadio } from '../../components/index';
import ListCard from "../../components/Cards/ListCard";
import CreateListCard from "../../components/Cards/CreateCard";
import ListEditCard from "../../components/Cards/ListEditCard";
import ListUsersCard from "../../components/Cards/ListUsersCard";
import ListTextCard from "../../components/Cards/ListTextCard";
import PerfectScrollbar from "perfect-scrollbar";
import { Select, Option } from 'element-ui';
import WindowPopup from "../../components/Popups/WindowPopup";
import { Sortable } from '@shopify/draggable';
import draggable from "vuedraggable";
import LoadingOverlay from "../../components/LoadingOverlay";
import SavePopup from "../../components/Popups/SavePopup";

export default {
  components: {
    SavePopup,
    LoadingOverlay,
    WindowPopup,
    ListTextCard,
    ListUsersCard,
    ListEditCard,
    ListCard,
    CreateListCard,
    BaseCheckbox,
    BaseRadio,
    [Option.name]: Option,
    [Select.name]: Select,
    draggable
  },
  computed: {
    categoriesAvailableToAdd() {
      return this.categories.filter((category) => {
        return !this.list.categories.some((listCategory) => { return listCategory.id === category.id });
      });
    },
    versionsUiVisible() {
      return this.showVersionsUi || this.list.versions.length > 1;
    },
    hideVersionsUiButtonVisible() {
      return this.list.versions.length === 1 && this.list.versions[0].display_name === 'По умолчанию';
    },
    currentListText() {
      return (this.selectedVersionForText.hasOwnProperty('texts') && this.selectedVersionForText.texts.length > 0) ? this.selectedVersionForText.texts[0].text : 'Добавьте макет и сохраните изменения для просмотра текста списка.';
    },
    isCurrentListTextAvailable() {
      return this.selectedVersionForText.hasOwnProperty('texts')  && this.selectedVersionForText.texts.length > 0;
    },
    sortedListCategories() {
      return this.list.categories.sort((a, b) => a.position > b.position ? 1 : -1);
    },
    isSavePopupVisible: function() {
      return !_.isEqual(this.list, this.listBackup) || !_.isEqual(this.usersWithoutCategory, this.usersWithoutCategoryBackup);
    }
  },
  data() {
    return {
      ps: {},
      //sortable: null,
      isSaving: false,
      sortableCategories: null,
      addCategoryPopupVisible: false,
      showVersionsUi: false,
      categoryToAdd: null,
      categoryToRemove: null,
      selectedVersionForEditing: {},
      selectedVersionForText: {},
      removeVersionPopupVisible: false,
      usersCardVisible: true,
      tooltipVisible: false,
      removeUserPopupVisible: false,
      removeCategoryPopupVisible: false,
      userToRemove: {},
      list: null,
      listBackup: null,
      usersWithoutCategory: [],
      usersWithoutCategoryBackup: [],
      categories: null,
    }
  },
  methods: {
    copy() {
      document.getElementById('listtext-textarea').select();
      document.execCommand("copy");

      this.tooltipVisible = true;
      document.vm = this;
      setTimeout(function () {
        document.vm.tooltipVisible = false;
      }, 1500);
    },
    adjustTextarea(id) {
      let layoutTextarea = document.getElementById(id);
      if (!layoutTextarea) return;
      layoutTextarea.style.height = "0";
      layoutTextarea.style.height = layoutTextarea.scrollHeight + 5 + "px";
      layoutTextarea.style.width = "0";
      layoutTextarea.style.width = layoutTextarea.scrollWidth + 5 + "px";
      this.ps[id].update();
    },
    createVersion() {
        let min = _.min(_.map(this.list.versions, 'id'));
        let id = (min < 0 ? min : 0) - 1;
        let version = {id: id, name: 'new_version', display_name: 'Новая версия', layout: ''};
        this.list.versions.push(version);
        this.selectedVersionForEditing = version;
    },
    removeVersion() {
      this.list.versions.splice(this.list.versions.indexOf(this.selectedVersionForEditing));
      this.selectedVersionForEditing = this.list.versions[0];
    },
    removeUser() {
      this.list.categories.forEach(c => {
        let userIndex = c.users.findIndex(u => u.id === this.userToRemove.id);
        if (userIndex !== -1) {
          c.users.splice(userIndex, 1);
        }
      });
    },
    getUsersByCategoryId(categoryId) {
      return _.filter(this.list.users, (user) => user.category_id === categoryId).sort((a, b) => a.position > b.position ? 1 : -1);
    },
    addCategory() {
      this.addCategoryPopupVisible = false;
      this.categoryToAdd.position = Math.max(...this.list.categories.map((c) => c.position)) + 1;
      this.categoryToAdd.users = [];
      this.list.categories.push(JSON.parse(JSON.stringify(this.categoryToAdd)));
      this.categoryToAdd = null;
    },
    removeCategory() {
      this.removeCategoryPopupVisible = false;
      this.categoryToRemove.users.forEach(u => {
        u.category_id = null;
      });
      this.usersWithoutCategory.push(...this.categoryToRemove.users);
      this.list.categories.splice(this.list.categories.indexOf(this.categoryToRemove), 1);
      this.categoryToRemove = null;
    },
    refresh() {
      return Promise.all([
      this.$api.get('servers/' + this.$route.params.id + '/categories/')
        .then(response => {
          this.categories = response.data.data;
        }),
      this.$api.get('servers/' + this.$route.params.id + '/lists/' + this.$route.params.list_id)
        .then(response => {
          this.selectedVersionForEditing = response.data.data.versions[0];
          this.selectedVersionForText = response.data.data.versions[0];
          this.list = response.data.data;
          this.listBackup = JSON.parse(JSON.stringify(this.list));
          this.usersWithoutCategoryBackup = JSON.parse(JSON.stringify(this.usersWithoutCategory));
        })
      ]);
    },
    save() {
      // TODO: restore selected versions.
      this.isSaving = true;
      let promises = [];

      let plainList = JSON.parse(JSON.stringify(this.list)), plainListBackup = JSON.parse(JSON.stringify(this.listBackup)); // lists without any relations, such as versions or categories
      ['categories', 'versions'].forEach((key) => {
        delete plainList[key];
        delete plainListBackup[key];
      });
      if (!_.isEqual(plainList, plainListBackup)) {
        promises.push(this.$api.put('servers/' + this.$route.params.id + '/lists/' + this.$route.params.list_id, plainList));
      }

      let versionsToCreate = _.differenceWith(this.list.versions, this.listBackup.versions, _.isEqual).filter((version) => !this.listBackup.versions.some((backupVersion) => version.id === backupVersion.id));
      versionsToCreate.forEach((v) => { promises.push(this.$api.post('servers/' + this.$route.params.id + '/lists/' + this.$route.params.list_id + '/versions', { name: v.name, display_name: v.display_name, layout: v.layout })) });
      let versionsToUpdate = _.differenceWith(this.list.versions, this.listBackup.versions, _.isEqual).filter((version) => this.listBackup.versions.some((backupVersion) => version.id === backupVersion.id));
      versionsToUpdate.forEach((v) => { promises.push(this.$api.put('servers/' + this.$route.params.id + '/lists/' + this.$route.params.list_id + '/versions/' + v.id, { name: v.name, display_name: v.display_name, layout: v.layout })) });
      let versionsToRemove = _.differenceWith(this.listBackup.versions, this.list.versions, (a, b) => a.id === b.id);
      versionsToRemove.forEach((v) => { promises.push(this.$api.delete('servers/' + this.$route.params.id + '/lists/' + this.$route.params.list_id + '/versions/' + v.id)); });

      let categoryPosition = 1, userPosition = 1;
      this.list.categories.forEach(c => {
        c.position = categoryPosition++;
        c.users.forEach(u => {
          u.position = userPosition++;
        });
      });

      let promise;
      if (!_.isEqual(this.list.categories, this.listBackup.categories)) {
        promise = this.$api.put('servers/' + this.$route.params.id + '/lists/' + this.$route.params.list_id + '/categories', {categories: this.list.categories});
      }

      if (this.usersWithoutCategory.length !== 0) {
        if (promise) {
          promise.then(() => {
            promise = this.$api.post('servers/' + this.$route.params.id + '/lists/' + this.$route.params.list_id + '/users', {users: this.usersWithoutCategory});
          });
        } else {
          promise = this.$api.post('servers/' + this.$route.params.id + '/lists/' + this.$route.params.list_id + '/users', {users: this.usersWithoutCategory});
        }
      }

      if (promise) {
        promises.push(promise);
      }

      Promise.all(promises).then(() => {
        this.refresh().then(() => {
          this.usersWithoutCategory = [];
          this.isSaving = false;
        });
      });
    },
    cancel() {
      this.list = JSON.parse(JSON.stringify(this.listBackup));
      this.usersWithoutCategory = JSON.parse(JSON.stringify(this.usersWithoutCategoryBackup));
      this.selectedVersionForEditing = this.list.versions[0];
      this.selectedVersionForText = this.list.versions[0];

      this.isSaving = false;
    }

  },
  created() {

  },
  mounted() {
    this.refresh().then(() => {
      this.ps['listtext-textarea'] = new PerfectScrollbar('#listtext-textarea-wrapper', {
        wheelPropagation: false,
      });
      this.ps['layout-textarea'] = new PerfectScrollbar('#layout-textarea-wrapper', {
        wheelPropagation: false,
      });

      setTimeout(() =>
      {

        this.adjustTextarea('listtext-textarea');
        this.ps['listtext-textarea'].update();

        this.adjustTextarea('layout-textarea');
        this.ps['layout-textarea'].update();
      }, 100);

      this.selectedVersionForEditing = this.list.versions[0];
    });
  },
  watch: {
    selectedVersionForText: function(newVal, oldVal) {
      setTimeout(() => {
        this.adjustTextarea('listtext-textarea');
      }, 10);
    }
  }
};
</script>
<style scoped lang="scss">

</style>
