<?php

namespace App\Http\Requests\v1\Categories;

use App\Enums\ApiErrorCode;
use App\Exceptions\ApiException;
use Illuminate\Contracts\Validation\Validator;
use Illuminate\Foundation\Http\FormRequest;
use Illuminate\Support\Facades\Log;
use Illuminate\Validation\Rule;

class UpdateRequest extends FormRequest
{
    /**
     * Determine if the user is authorized to make this request.
     *
     * @return bool
     */
    public function authorize()
    {
        return true;
    }

    /**
     * Get the validation rules that apply to the request.
     *
     * @return array
     */
    public function rules()
    {
        return [
            'name' => [
                'nullable',
                'string',
                'max:128',
                Rule::unique('categories', 'name')->where('server_id', $this->route()->parameter('server')->id)->ignore($this->route()->parameter('category')->id),
            ],
            'icon' => [
                'nullable',
                'string',
                'max:8',
            ],
            'posts_count' => [
                'nullable',
                'integer',
                'min:0'
            ],
            'properties' => [
                'nullable',
                function ($attribute, $properties, $fail) {
                    $allProperties = ['mix'];

                    $unknownProperties = array_diff($properties, $allProperties);
                    if (!empty($unknownProperties)) {
                        $fail('Unknown properties: ' . implode(', ', $unknownProperties) . '.');
                    }
                }
            ],
            'default_for_roles.*' => [
                'exists:roles,id',
            ],
        ];
    }

    protected function getValidatorInstance()
    {
        Log::debug(print_r($this->request->get('default_for_roles'), true));

        $this->parseProperties();
        $this->parseRoles();

        return parent::getValidatorInstance();
    }

    protected function parseProperties() {
        $this->merge(['properties' => array_filter(explode(',', trim($this->request->get('properties'))), 'strlen')]);
    }

    protected function parseRoles() {
        $this->merge(['default_for_roles' => array_filter(explode(',', trim($this->request->get('default_for_roles'))), 'strlen')]);
    }

    function failedValidation(Validator $validator)
    {
        throw new ApiException(ApiErrorCode::VALIDATION_FAILED, $validator->errors()->messages(), 422);
    }
}
