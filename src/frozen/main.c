#include <libfrozen.h>
#include <mustache.h>

/**
 * @ingroup backend
 * @addtogroup mod_backend_mustache module/mustache
 */
/**
 * @ingroup mod_backend_mustache
 * @page page_mustache_info Description
 *
 */
/**
 * @ingroup mod_backend_mustache
 * @page page_mustache_config Configuration
 *  
 * Accepted configuration:
 * @code
 * {
 *              class                   = "modules/mustache",
 *              template                = (backend_t){                         # template to use
 *                  { <some backend with template info> }
 *              },
 *              output                  = (hashkey_t)"buffer"                  # output result to, default "output"
 * }
 * @endcode
 */

#define EMODULE 103

#define HK_VALUE_template 21633 // TODO replace this with HDK

typedef struct mustache_userdata {
	mustache_template_t   *template;
	data_t                *tpl_data;
	hashkey_t              output;
} mustache_userdata;

typedef struct mustache_ctx {
	backend_t             *backend;
	request_t             *request;
	mustache_userdata     *userdata;
	data_t                *output;
	
	uintmax_t              error_lineno;
	char                  *error_msg;
} mustache_ctx;

typedef struct token_userdata {
	hashkey_t              key;
} token_userdata;

uintmax_t mustache_frozen_read   (mustache_api_t *api, mustache_userdata *userdata, char *buffer, uintmax_t buffer_size){ // {{{
	fastcall_read r_read = { { 5, ACTION_READ }, 0, buffer, buffer_size };
	if( data_query(userdata->tpl_data, &r_read) < 0 )
		return 0;
	
	return r_read.buffer_size;
} // }}}
uintmax_t mustache_frozen_write  (mustache_api_t *api, mustache_ctx *ctx, char *buffer, uintmax_t buffer_size){ // {{{
	fastcall_write r_write = { { 5, ACTION_WRITE }, 0, buffer, buffer_size };
	if( data_query(ctx->output, &r_write) < 0 )
		return 0;
	
	return r_write.buffer_size;
} // }}}
uintmax_t mustache_frozen_varget (mustache_api_t *api, mustache_ctx *ctx, mustache_token_variable_t *token){ // {{{
	data_t                *data;
	
	if( (data = hash_data_find(ctx->request, ((token_userdata *)token->userdata)->key)) == NULL)
		return 0;
	
	fastcall_transfer r_transfer = { { 4, ACTION_TRANSFER }, ctx->output };
	if(data_query(data, &r_transfer) < 0)
		return 0;
	
	return r_transfer.transfered;
} // }}}
uintmax_t mustache_frozen_sectget(mustache_api_t *api, mustache_ctx *ctx, mustache_token_section_t  *token){ // {{{
	return 1;
} // }}}
void      mustache_frozen_error  (mustache_api_t *api, mustache_ctx *ctx, uintmax_t lineno, char *error){ // {{{
	ctx->error_lineno = lineno;
	ctx->error_msg    = strdup(error);
} // }}}
void      mustache_frozen_freedata (mustache_api_t *api, void *userdata){ // {{{
	free(userdata);
} // }}}

uintmax_t mustache_frozen_prevarget (mustache_api_t *api, mustache_ctx *ctx, mustache_token_variable_t *token){ // {{{
	hashkey_t              hashkey;
	data_t                 hashkey_str       = DATA_RAW(token->text, token->text_length);
	data_t                 hashkey_d         = DATA_PTR_HASHKEYT(&hashkey);
	
	fastcall_convert_from r_convert = { { 4, ACTION_CONVERT_FROM }, &hashkey_str, FORMAT(config) };
	if(data_query(&hashkey_d, &r_convert) < 0)
		return 0;
	
	if( (token->userdata = malloc(sizeof(token_userdata))) == NULL)
		return 0;
	
	((token_userdata *)token->userdata)->key = hashkey;
	return 1;
} // }}}
uintmax_t mustache_frozen_presectget(mustache_api_t *api, mustache_ctx *ctx, mustache_token_section_t  *token){ // {{{
	return 1;
} // }}}

mustache_api_t mustache_api = {
	.read     = (mustache_api_read)&mustache_frozen_read,
	.write    = (mustache_api_write)&mustache_frozen_write,
	.varget   = (mustache_api_varget)&mustache_frozen_varget,
	.sectget  = (mustache_api_sectget)&mustache_frozen_sectget,
	.error    = (mustache_api_error)&mustache_frozen_error,
	.freedata = (mustache_api_freedata)&mustache_frozen_freedata
};

static int mustache_init(backend_t *backend){ // {{{
	mustache_userdata         *userdata;

	if((userdata = backend->userdata = calloc(1, sizeof(mustache_userdata))) == NULL)
		return error("calloc failed");
	
	userdata->output = HK(output);
	return 0;
} // }}}
static int mustache_destroy(backend_t *backend){ // {{{
	mustache_userdata     *userdata          = (mustache_userdata *)backend->userdata;
	
	if(userdata->template)
		mustache_free(&mustache_api, userdata->template);
	
	free(userdata);
	return 0;
} // }}}
static int mustache_configure(backend_t *backend, config_t *config){ // {{{
	ssize_t                ret;
	data_t                *tpl_data;
	mustache_userdata     *userdata          = (mustache_userdata *)backend->userdata;
	
	hash_data_copy(ret, TYPE_HASHKEYT, userdata->output, config, HK(output));
	
	if( (tpl_data = hash_data_find(config, HK(template))) == NULL)
		return -EINVAL;
	
	data_t dslide = DATA_SLIDERT(tpl_data, 0);
	userdata->tpl_data = &dslide;
	
	if( (userdata->template = mustache_compile(&mustache_api, userdata)) == NULL)
		return error("bad template");
	
	mustache_api_t pre_api = {
		.varget  = (mustache_api_varget)&mustache_frozen_prevarget,
		.sectget = (mustache_api_sectget)&mustache_frozen_presectget
	};
	if( (mustache_prerender(&pre_api, userdata, userdata->template)) == 0)
		return error("bad precompile");
	
	return 0;
} // }}}

static ssize_t mustache_handler(backend_t *backend, request_t *request){ // {{{
	ssize_t                ret;
	data_t                *output;
	mustache_userdata     *userdata = (mustache_userdata *)backend->userdata;
	
	if( (output = hash_data_find(request, userdata->output)) == NULL)
		return error("buffer not supplied");
	
	data_t                 dslide   = DATA_SLIDERT(output, 0);
	mustache_ctx           ctx      = { backend, request, userdata, &dslide };
	
	if(mustache_render(&mustache_api, &ctx, userdata->template) == 0)
		return -EFAULT;
	
	return ((ret = backend_pass(backend, request)) < 0) ? ret : -EEXIST;
} // }}}

static backend_t c_mustache_proto = {
	.class          = "modules/mustache",
	.supported_api  = API_HASH,
	.func_init      = &mustache_init,
	.func_configure = &mustache_configure,
	.func_destroy   = &mustache_destroy,
	.backend_type_hash = {
		.func_handler = &mustache_handler
	}
};

int main(void){
	class_register(&c_mustache_proto);
	return 0;
}
