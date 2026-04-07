using UnityEngine;

public class BossAttack : MonoBehaviour
{
    public GameObject fireballPrefab;
    public Transform firePoint;

    public GameObject meleeHitbox;
    public Transform meleePoint;

    public GameObject skeletonPrefab;
    public Transform leftSpawn;
    public Transform rightSpawn;

    public void ChooseAttack()
    {
        int rand = Random.Range(0, 3);

        if (rand == 0)
            RangedAttack();
        else if (rand == 1)
            MeleeAttack();
        else
            Summon();
    }

    //  Attaque à distance
    void RangedAttack()
    {
        GetComponent<Animator>().SetTrigger("Ranged");
        Invoke(nameof(SpawnFireball), 0.4f);
    }

    void SpawnFireball()
    {
        Instantiate(fireballPrefab, firePoint.position, Quaternion.identity);
    }

    //  Corps à corps
    void MeleeAttack()
    {
        GetComponent<Animator>().SetTrigger("Melee");
        Invoke(nameof(ActivateMelee), 0.3f);
    }

    void ActivateMelee()
    {
        GameObject hit = Instantiate(meleeHitbox, meleePoint.position, Quaternion.identity);
        Destroy(hit, 0.2f);
    }

    //  Invocation
    void Summon()
    {
        GetComponent<Animator>().SetTrigger("Summon");
        Invoke(nameof(SpawnSkeletons), 0.5f);
    }

    void SpawnSkeletons()
    {
        Instantiate(skeletonPrefab, leftSpawn.position, Quaternion.identity);
        Instantiate(skeletonPrefab, rightSpawn.position, Quaternion.identity);
    }
}

// projectile
using UnityEngine;

public class Fireball : MonoBehaviour
{
    public float speed = 6f;
    public int damage = 10;

    void Update()
    {
        transform.Translate(Vector2.right * speed * Time.deltaTime);
    }

    void OnTriggerEnter2D(Collider2D col)
    {
        if (col.CompareTag("Player"))
        {
            col.GetComponent<PlayerHealth>().TakeDamage(damage);
            Destroy(gameObject);
        }
    }
}

// hitbox corp a corp
using UnityEngine;

public class MeleeHitbox : MonoBehaviour
{
    public int damage = 15;

    void OnTriggerEnter2D(Collider2D col)
    {
        if (col.CompareTag("Player"))
        {
            col.GetComponent<PlayerHealth>().TakeDamage(damage);
        }
    }
}

//  mob invcation
using UnityEngine;

public class Skeleton : MonoBehaviour
{
    public float speed = 2f;

    void Update()
    {
        transform.Translate(Vector2.left * speed * Time.deltaTime);
    }
}

// fonctionnement du boss
using UnityEngine;

public class BossAI : MonoBehaviour
{
    public Transform player;

    [Header("Distances")]
    public float meleeRange = 2f;
    public float attackCooldown = 2f;

    [Header("Health")]
    public int maxHealth = 100;
    private int currentHealth;

    [Header("Prefabs")]
    public GameObject fireballPrefab;
    public Transform firePoint;

    public GameObject meleeHitbox;
    public Transform meleePoint;

    public GameObject skeletonPrefab;
    public Transform leftSpawn;
    public Transform rightSpawn;

    private float cooldownTimer;
    private bool hasSummoned = false;

    private Animator anim;

    void Start()
    {
        currentHealth = maxHealth;
        anim = GetComponent<Animator>();
    }

    void Update()
    {
        if (player == null) return;

        cooldownTimer -= Time.deltaTime;

        float distance = Vector2.Distance(transform.position, player.position);

        if (cooldownTimer <= 0)
        {
            //  Invocation si moins de 50% HP (UNE SEULE FOIS)
            if (!hasSummoned && currentHealth <= maxHealth / 2)
            {
                Summon();
                hasSummoned = true;
            }
            else
            {
                //  Choix attaque selon distance
                if (distance <= meleeRange)
                {
                    MeleeAttack();
                }
                else
                {
                    RangedAttack();
                }
            }

            cooldownTimer = attackCooldown;
        }
    }

    //  Attaque à distance
    void RangedAttack()
    {
        anim.SetTrigger("Ranged");
        Invoke(nameof(SpawnFireball), 0.4f);
    }

    void SpawnFireball()
    {
        Instantiate(fireballPrefab, firePoint.position, Quaternion.identity);
    }

    //  Corps à corps
    void MeleeAttack()
    {
        anim.SetTrigger("Melee");
        Invoke(nameof(SpawnMelee), 0.3f);
    }

    void SpawnMelee()
    {
        GameObject hit = Instantiate(meleeHitbox, meleePoint.position, Quaternion.identity);
        Destroy(hit, 0.2f);
    }

    //  Invocation
    void Summon()
    {
        anim.SetTrigger("Summon");
        Invoke(nameof(SpawnSkeletons), 0.5f);
    }

    void SpawnSkeletons()
    {
        Instantiate(skeletonPrefab, leftSpawn.position, Quaternion.identity);
        Instantiate(skeletonPrefab, rightSpawn.position, Quaternion.identity);
    }

    //  Gestion des dégâts
    public void TakeDamage(int dmg)
    {
        currentHealth -= dmg;
    }
}